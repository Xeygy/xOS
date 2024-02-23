#include <stdint.h>
#include "print.h"
#include "string.h"
#include "asm.h"

#define ALIGN_8_BYTE(val) ((val + 7) / 8 ) * 8 
#define PF_SIZE_BYTES 4096
#define GIGABYTE 0x40000000

// https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html#Boot-information-format
typedef enum {
    TYPE_END_TAG_LIST = 0, // must be size 8 to end
	TYPE_BOOT_CMD_LINE = 1,
	TYPE_BOOTLDR_NAME = 2,
	TYPE_BASIC_MEM_INFO = 4,
	TYPE_BIOS_BOOT_DEVICE = 5,
	TYPE_MMAP = 6,
	TYPE_ELF_SYMS = 9,
} mb2_tag_t;

// Multiboot Info Header
typedef struct mb2_info_hdr {
    uint32_t totalSizeBytes;
	uint32_t reservedMBZ;
} __attribute__((packed)) mb2_info_hdr;

// Multiboot Tag Header
typedef struct mb2_tag_hdr {
    uint32_t tagType;
	uint32_t totalSizeBytes;
} __attribute__((packed)) mb2_tag_hdr;

// expected mmap_entry
typedef struct mmap_entry {
    uint64_t start_addr;
	uint64_t byte_len;
    uint16_t type;
    uint32_t reserved:20;
} __attribute__((packed)) mmap_entry;

typedef struct elf_syms_hdr {
    uint32_t tagType;
	uint32_t totalSizeBytes;
    uint32_t numEntries;
	uint32_t entryByteSize;
    uint32_t strTblIdx;
} __attribute__((packed)) elf_syms_hdr;

typedef struct elf_syms_entry {
    uint32_t nameIdx;  // index into the string table
	uint32_t sectionType;
    uint64_t flags;
    uint64_t memAddr;
    uint64_t offsetOnDisk;
    uint64_t segmentByteSize;
    uint32_t tableIdxLink;
    uint32_t extra;
    uint64_t addrAlignement;
    uint64_t IFFsize;
} __attribute__((packed)) elf_syms_entry;

/* page frame, the first sizeof bits are holding 
the pointer to the next page frame */
typedef struct pf_hdr {
    uint64_t next;
} __attribute__((packed)) pf_hdr;

typedef struct mem_chunk {
    uint64_t addr;
    uint64_t byteSize;
} __attribute__((packed)) mem_chunk;

/* page table entry */
typedef struct pte {
    uint64_t present:1;
    uint64_t read_write:1;
    uint64_t user:1;
    uint64_t write_through:1;
    uint64_t cache_disable:1;
    uint64_t accessed:1;
    uint64_t dirty:1;   // misc flags
    uint64_t huge_pg:1; // misc flags
    uint64_t global:1;  // misc flags
    uint64_t demand:1;  // custom flag, set when a page has been alloc'd
    uint64_t avail1:2;
    uint64_t base_addr:40;
    uint64_t avail2:11;
    uint64_t nx:1;
} __attribute__((packed)) pte;

static mem_chunk mem_tbl[16]; // table of available mem
static uint64_t pf_total_ct;
static int mem_tbl_next_idx; // pointer to next free spot in mem_tbl

static uint64_t next_consecutive_pf_idx; // alloc
static pf_hdr *free_frames;

static pte *pt4;
//static uint64_t kheap_size; // number of pages in heap

static void fill_mem_tbl(void* mmap_tag);
static void reserve_elf_syms(void* syms_tag);
static void reserve_mem_tbl(uint64_t start, uint64_t end);
static int init_pf_alloc(void *mb2_head);
static int init_page_table();
static void * MMU_pf_alloc_and_clean();
//static void *vpage_alloc(uint64_t virt_addr);
//static void vpage_free(uint64_t virt_addr);

/* return a pointer to a page frame, returns 0 on error */
void * MMU_pf_alloc() {
    uint64_t offset = next_consecutive_pf_idx * PF_SIZE_BYTES;
    pf_hdr *ret;
    int i;
    if (pf_total_ct > next_consecutive_pf_idx) {
        // we can just alloc the next consecutive spot in memory
        for (i = 0; i < mem_tbl_next_idx; i++) {
            if (mem_tbl[i].byteSize > offset) {
                // this is the right section
                next_consecutive_pf_idx++;
                return (void *) mem_tbl[i].addr + offset;
            }
            offset -= mem_tbl[i].byteSize;
        }
        printk("ERROR: pf_alloc shouldn't get here, pf_total_ct may be inaccurate");
        return 0;
    } else {
        // start using the free frames list
        if (free_frames == 0) {
            printk("pf_alloc ran out of memory");
            asm volatile ("hlt");
            return 0;
        }
        ret = free_frames;
        free_frames = (void *) free_frames->next;
        return ret;
    }
}

/* alloc a page frame, and set all values to 0, returns 0 on failure*/
static void * MMU_pf_alloc_and_clean() {
    void * pf;
    pf = MMU_pf_alloc();
    if (pf) {
        memset(pf, 0, PF_SIZE_BYTES);
    }
    return pf;
}

void MMU_pf_free(void *pf) {
    pf_hdr *pf_start, *tmp;

    // get the start of the PF_SIZE_BYTES block
    pf_start = pf - (uint64_t) pf % PF_SIZE_BYTES;

    // send pf_start to the start of the free_frames list
    tmp = free_frames;
    free_frames = pf_start;
    pf_start->next = (uint64_t) tmp; 
}


/* 
initialize our table of available memory into mem_tbl
returns 0 on success, error code on fail
*/
int MMU_init(void *mb2_head) {
    init_pf_alloc(mb2_head);
    init_page_table();
    return 0;
}

static int init_page_table() {
    int i, num_entries; 
    pte *pt3;
    num_entries = PF_SIZE_BYTES / 8; // 8 byte entries

    // clear all in pt4
    pt4 = MMU_pf_alloc();
    for (i = 0; i < num_entries; i++) {
        memset(pt4 + i, 0, 8);
        pt4[i].read_write = 1;
    }

    // give kernel first 16 pages

    // first table should be 1 to 1 mapping
    // this maps 512 * 512 * 2MB = up to 512GB RAM support
    pt4[0].base_addr = ((uint64_t) MMU_pf_alloc()) >> 12; // lvl 4->3
    pt4[0].present = 1;
    pt3 = (void *) (uint64_t) (pt4[0].base_addr << 12); 
    // outer loop for each entry in the pt3
    for (i = 0; i < PF_SIZE_BYTES/sizeof(pte); i++) {
        memset(pt3 + i, 0, 8);
        pt3[i].present = 1;
        pt3[i].base_addr = (i * GIGABYTE) >> 12; // 1GB huge pages
        pt3[i].huge_pg = 1;
        pt3[i].read_write = 1;
    }

    // call cr3
    asm volatile ("movq %0, %%cr3" 
                :
                : "r"(pt4)); 
    // mmio is in there???
    return 0;
}

// allocates the page at virt_addr 
void *vpage_alloc(uint64_t virt_addr) {
    uint64_t pt4_idx, pt3_idx, pt2_idx, pt1_idx;
    pte *pt3, *pt2, *pt1;

    // align virt_addr
    virt_addr = virt_addr & ~0xFFF;
    pt1_idx = (virt_addr >> 12) & 0x1FF;
    pt2_idx = (virt_addr >> 21) & 0x1FF;
    pt3_idx = (virt_addr >> 30) & 0x1FF;
    pt4_idx = (virt_addr >> 39) & 0x1FF;

    if (!pt4[pt4_idx].present) {
        memset(pt4 + pt4_idx, 0, 8);
        pt4[pt4_idx].base_addr = ((uint64_t) MMU_pf_alloc_and_clean()) >> 12;
        pt4[pt4_idx].present = 1;
        pt4[pt4_idx].read_write = 1;
    }
    pt3 = ((pte *) (uint64_t) (pt4[pt4_idx].base_addr << 12));
    if (!pt3[pt3_idx].present) {
        memset(pt3 + pt3_idx, 0, 8);
        pt3[pt3_idx].base_addr = ((uint64_t) MMU_pf_alloc_and_clean()) >> 12;
        pt3[pt3_idx].present = 1;
        pt3[pt3_idx].read_write = 1;
    }
    pt2 = ((pte *) (uint64_t) (pt3[pt3_idx].base_addr << 12));
    if (!pt2[pt2_idx].present) {
        memset(pt2 + pt2_idx, 0, 8);
        pt2[pt2_idx].base_addr = ((uint64_t) MMU_pf_alloc_and_clean()) >> 12;
        pt2[pt2_idx].present = 1;
        pt2[pt2_idx].read_write = 1;
    }
    pt1 = ((pte *) (uint64_t) (pt2[pt2_idx].base_addr << 12));
    if (!pt1[pt1_idx].present) {
        memset(pt1 + pt1_idx, 0, 8);
        // demand paging hack
        pt1[pt1_idx].demand = 1; 
        pt1[pt1_idx].read_write = 1;
    }
    return (void *) virt_addr;
}

// frees the page at virt_addr 
void vpage_free(uint64_t virt_addr) {
    uint64_t pt4_idx, pt3_idx, pt2_idx, pt1_idx;
    pte *pt3, *pt2, *pt1;
    pt1_idx = (virt_addr >> 12) & 0x1FF;
    pt2_idx = (virt_addr >> 21) & 0x1FF;
    pt3_idx = (virt_addr >> 30) & 0x1FF;
    pt4_idx = (virt_addr >> 39) & 0x1FF;

    if (!pt4[pt4_idx].present) {
        return;
    }
    pt3 = ((pte *) (uint64_t) (pt4[pt4_idx].base_addr << 12));
    if (!pt3[pt3_idx].present) {
        return;
    }
    pt2 = ((pte *) (uint64_t) (pt3[pt3_idx].base_addr << 12));
    if (!pt2[pt2_idx].present) {
        return;
    }
    pt1 = ((pte *) (uint64_t) (pt2[pt2_idx].base_addr << 12));
    if (!pt1[pt1_idx].present) {
        return;
    }
    MMU_pf_free((void*) (uint64_t) (pt1[pt1_idx].base_addr << 12));
    pt1[pt1_idx].present = 0;
    // let tlb know
    invlpg((void *) virt_addr);
}

/* takes a virtual address and returns the actual mem address 
static void* virt_addr_to_real(uint64_t virt_addr) {
    uint64_t offset4, offset3, offset2, offset1, offset_phys;
    offset4 = (virt_addr >> 39) & 0x1FF;
    offset3 = (virt_addr >> 30) & 0x1FF;
    offset2 = (virt_addr >> 21) & 0x1FF;
    offset1 = (virt_addr >> 12) & 0x1FF;
    offset_phys = (virt_addr >> 12) & 0xFFF;

    // first table should be 1 to 1 mapping
} */

/*
set up our pf allocator so we can use pf_alloc & free
returns 0 on success, error code on fail
*/
static int init_pf_alloc(void *mb2_head) {
    mb2_info_hdr* header = mb2_head;
    mb2_tag_hdr* curr_tag_hdr;
    int end_mb2, i;
    void *elf_syms, *mmap;
    void* curr_ptr = mb2_head + sizeof(*header);

    if (header->reservedMBZ != 0) {
        printk("mb2 header @ %p, had nonzero value 0x%x in zeros\n", mb2_head, header->reservedMBZ);
        return 1;
    }
    while (!end_mb2 && curr_ptr < mb2_head + header->totalSizeBytes) {
        curr_tag_hdr = curr_ptr;
        printk("tag type: %u, size: %u\n", curr_tag_hdr->tagType, curr_tag_hdr->totalSizeBytes);
        switch (curr_tag_hdr->tagType)
        {
        case TYPE_MMAP:
            mmap = curr_ptr;
            break;
        case TYPE_ELF_SYMS:
            elf_syms = curr_ptr;
            break;
        case TYPE_BOOTLDR_NAME:
            printk("Name: %s\n", (char *) (curr_ptr + 8));
            break;
        case TYPE_END_TAG_LIST:
            if (curr_tag_hdr->totalSizeBytes == 8){
                end_mb2 = 1;
            }
            break;
        default:
            break;
        }
        curr_ptr += ALIGN_8_BYTE(curr_tag_hdr->totalSizeBytes);
    }
    // get all available memory from mmap into mem_tbl
    fill_mem_tbl(mmap);
    // remove any memory that the kernel is using
    reserve_elf_syms(elf_syms);
    // remove first page frame bytes, so null isn't alloc'd
    reserve_mem_tbl(0, PF_SIZE_BYTES);

    for (i = 0; i < mem_tbl_next_idx; i++) {
        // store total page frame count in pf_total_ct
        pf_total_ct += mem_tbl[i].byteSize / PF_SIZE_BYTES;
        // make sure chunks are pf_size_bytes aligned
        if (mem_tbl[i].byteSize & (PF_SIZE_BYTES - 1)) {
            // round down
            mem_tbl[i].byteSize = (mem_tbl[i].byteSize / PF_SIZE_BYTES) * PF_SIZE_BYTES;
        }
        if (mem_tbl[i].addr & (PF_SIZE_BYTES - 1)) {
            // round up
            mem_tbl[i].addr = ((mem_tbl[i].addr + PF_SIZE_BYTES - 1) / PF_SIZE_BYTES ) * PF_SIZE_BYTES;
        }
    }
    return 0;
}

// fill mem table with mem chunks of type 1 from mmap
static void fill_mem_tbl(void* mmap_tag) {
    mb2_tag_hdr* header = mmap_tag;
    uint16_t size_of_mmap_entry = *(uint16_t*) (mmap_tag + 8);
    mmap_entry* curr_entry = mmap_tag + 16; 

    while ((void*) curr_entry < mmap_tag + header->totalSizeBytes) {
        if (curr_entry->type == 1) {
            printk("avail start %lx, end %lx\n", curr_entry->start_addr, curr_entry->start_addr + curr_entry->byte_len);
            mem_tbl[mem_tbl_next_idx].addr = curr_entry->start_addr;
            mem_tbl[mem_tbl_next_idx].byteSize = curr_entry->byte_len;
            mem_tbl_next_idx++;
        }
        curr_entry = (void *) curr_entry + size_of_mmap_entry;
    }
}

// carve out any mem spots being used in elf symbols from mem table
static void reserve_elf_syms(void* syms_tag) {
    elf_syms_hdr* header = syms_tag;
    elf_syms_entry* curr_entry = syms_tag + sizeof(elf_syms_hdr);
    uint64_t curr_chunk_start = 0, curr_chunk_end = 0;

    while ((void *) curr_entry < syms_tag + header->totalSizeBytes) {
        // relies on sequential chunk order
        if (curr_entry->memAddr > curr_chunk_end + PF_SIZE_BYTES) { // we will just connect chunks less than a page frame apart
            printk("mem start %lx, end %lx\n", curr_chunk_start, curr_chunk_end);
            reserve_mem_tbl(curr_chunk_start, curr_chunk_end);
            curr_chunk_start = curr_entry->memAddr;
        } 
        curr_chunk_end = curr_entry->memAddr + curr_entry->segmentByteSize;
        curr_entry = (void *) curr_entry + header->entryByteSize;
    }
    printk("last mem start %lx, end %lx\n", curr_chunk_start, curr_chunk_end);
    reserve_mem_tbl(curr_chunk_start, curr_chunk_end);
}

// truncates memory in mem table based on [start, end)
static void reserve_mem_tbl(uint64_t start, uint64_t end) {
    int i, j;
    mem_chunk* curr;

    for (i = 0; i < mem_tbl_next_idx; i++) {
        curr = &mem_tbl[i];
        
        if (start <= curr->addr && end >= curr->addr + curr->byteSize) {
            // fully overlapped
            // remove mem from table, shift everything down 1
            mem_tbl_next_idx--;
            for (j = i; j < mem_tbl_next_idx; j++) {
                mem_tbl[j] = mem_tbl[j + 1];
            }
            mem_tbl[j + 1].addr = 0;
            mem_tbl[j + 1].byteSize = 0;
        } else if (start <= curr->addr && end > curr->addr) {
            // overlaps front edge of the chunk
            curr -> byteSize = curr->byteSize - (end - curr->addr);
            curr -> addr = end;
        } else if (start < curr->addr + curr->byteSize && end >= curr->addr + curr->byteSize) {
            // overlaps back edge of the chunk
            curr -> byteSize -= curr->addr + curr->byteSize - start;
        } else if (start > curr->addr && end < curr->addr + curr->byteSize) {
            // mem we want to removes is in the middle of the chunk
            // store back segment in next available spot
            mem_tbl[mem_tbl_next_idx].byteSize = curr->byteSize - (end - curr->addr);
            mem_tbl[mem_tbl_next_idx].addr = end;
            mem_tbl_next_idx++;

            // store front segment in the current entry
            curr -> byteSize -= curr->addr + curr->byteSize - start;
        }
    }
}

/* handles a page fault */
void pf_isr(uint64_t error_code) {
    uint64_t pt4_idx, pt3_idx, pt2_idx, pt1_idx;
    pte *pt3, *pt2, *pt1;
    uint64_t faulting_addr = 0xBEEFCAFE;
    // read 
    asm volatile ("movq %%cr2, %0"
                : "=r" (faulting_addr)); 
    printk("page fault on %lx, error code %lx\n", faulting_addr, error_code);
    pt1_idx = (faulting_addr >> 12) & 0x1FF;
    pt2_idx = (faulting_addr >> 21) & 0x1FF;
    pt3_idx = (faulting_addr >> 30) & 0x1FF;
    pt4_idx = (faulting_addr >> 39) & 0x1FF;

    if (!pt4[pt4_idx].present) {
        printk("level 4 not present\n");
        asm volatile ("hlt");
    }
    pt3 = ((pte *) (uint64_t) (pt4[pt4_idx].base_addr << 12));
    if (!pt3[pt3_idx].present) {
        printk("level 3 not present\n");
        asm volatile ("hlt");
    }
    pt2 = ((pte *) (uint64_t) (pt3[pt3_idx].base_addr << 12));
    if (!pt2[pt2_idx].present) {
        printk("level 2 not present\n");
        asm volatile ("hlt");
    }
    pt1 = ((pte *) (uint64_t) (pt2[pt2_idx].base_addr << 12));
    if (pt1[pt1_idx].demand) {
        // demand paging
        pt1[pt1_idx].base_addr = ((uint64_t) MMU_pf_alloc_and_clean()) >> 12;
        pt1[pt1_idx].present = 1;
        pt1[pt1_idx].demand = 0;
    }else if (pt1[pt1_idx].present) {
        printk("level 1 present\n");
        asm volatile ("hlt");
    } else {
        printk("level 1 not present\n");
        asm volatile ("hlt");
    }
    return;
}