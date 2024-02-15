#include <stdint.h>
#include "print.h"

#define ALIGN_8_BYTE(val) ((val + 7) / 8 ) * 8 
#define PF_SIZE_BYTES 4096

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

typedef struct mem_chunk {
    uint64_t addr;
    uint64_t byteSize;
} __attribute__((packed)) mem_chunk;

static mem_chunk mem_tbl[16]; // table of available mem
static int mem_tbl_next_idx; // pointer to next free spot in mem_tbl

static void fill_mem_tbl(void* mmap_tag);
static void reserve_mem_tbl(void* syms_tag);
static void remove_mem_tbl(uint64_t start, uint64_t end);

// initialize our table of available memory into mem_tbl
void MMU_init(void *mb2_head) {
    mb2_info_hdr* header = mb2_head;
    mb2_tag_hdr* curr_tag_hdr;
    int end_mb2;
    void *elf_syms, *mmap;
    void* curr_ptr = mb2_head + sizeof(*header);

    if (header->reservedMBZ != 0) {
        printk("mb2 header @ %p, had nonzero value 0x%x in zeros\n", mb2_head, header->reservedMBZ);
        return;
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
    reserve_mem_tbl(elf_syms);
}

// fill mem table with mem chunks of type 1 from mmap
static void fill_mem_tbl(void* mmap_tag) {
    mb2_tag_hdr* header = mmap_tag;
    uint16_t size_of_mmap_entry = *(uint16_t*) (mmap_tag + 8);
    mmap_entry* curr_entry = mmap_tag + 16; 

    while ((void*) curr_entry < mmap_tag + header->totalSizeBytes) {
        if (curr_entry->type == 1) {
            printk("avail start %lu, end %lu\n", curr_entry->start_addr, curr_entry->start_addr + curr_entry->byte_len);
            mem_tbl[mem_tbl_next_idx].addr = curr_entry->start_addr;
            mem_tbl[mem_tbl_next_idx].byteSize = curr_entry->byte_len;
            mem_tbl_next_idx++;
        }
        curr_entry = (void *) curr_entry + size_of_mmap_entry;
    }
}

// carve out any mem spots being used in elf symbols from mem table
static void reserve_mem_tbl(void* syms_tag) {
    elf_syms_hdr* header = syms_tag;
    elf_syms_entry* curr_entry = syms_tag + sizeof(elf_syms_hdr);
    uint64_t curr_chunk_start = 0, curr_chunk_end = 0;

    while ((void *) curr_entry < syms_tag + header->totalSizeBytes) {
        // relies on sequential chunk order
        if (curr_entry->memAddr > curr_chunk_end + PF_SIZE_BYTES) { // we will just connect chunks less than a page frame apart
            printk("mem start %lu, end %lu\n", curr_chunk_start, curr_chunk_end);
            remove_mem_tbl(curr_chunk_start, curr_chunk_end);
            curr_chunk_start = curr_entry->memAddr;
        } 
        curr_chunk_end = curr_entry->memAddr + curr_entry->segmentByteSize;
        curr_entry = (void *) curr_entry + header->entryByteSize;
    }
    printk("last mem start %lu, end %lu\n", curr_chunk_start, curr_chunk_end);
    remove_mem_tbl(curr_chunk_start, curr_chunk_end);
}

// truncates memory in mem table based on [start, end)
static void remove_mem_tbl(uint64_t start, uint64_t end) {
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

void * MMU_pf_alloc() {
    return 0;
}


