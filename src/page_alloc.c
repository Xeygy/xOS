#include <stdint.h>
#include "print.h"

#define ALIGN_8_BYTE(val) ((val + 7) / 8 ) * 8 

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

static void read_MMAP(void* mmap_tag);
static void read_ELF_syms(void* syms_tag);

void MMU_init(void *mb2_head) {
    mb2_info_hdr* header = mb2_head;
    mb2_tag_hdr* curr_tag_hdr;
    void* curr_ptr = mb2_head + sizeof(*header);

    if (header->reservedMBZ != 0) {
        printk("mb2 header @ %p, had nonzero value 0x%x in zeros\n", mb2_head, header->reservedMBZ);
        return;
    }
    while (curr_ptr < mb2_head + header->totalSizeBytes) {
        curr_tag_hdr = curr_ptr;
        printk("tag type: %u, size: %u\n", curr_tag_hdr->tagType, curr_tag_hdr->totalSizeBytes);
        switch (curr_tag_hdr->tagType)
        {
        case TYPE_MMAP:
            read_MMAP(curr_ptr);
            break;

        case TYPE_ELF_SYMS:
            read_ELF_syms(curr_ptr);
            break;

        case TYPE_BOOTLDR_NAME:
            printk("Name: %s\n", (char *) (curr_ptr + 8));
            break;

        case TYPE_END_TAG_LIST:
            if (curr_tag_hdr->totalSizeBytes == 8){
                return;
            }
            break;
        default:
            break;
        }
        curr_ptr += ALIGN_8_BYTE(curr_tag_hdr->totalSizeBytes);
    }
}

static void read_ELF_syms(void* syms_tag) {
    elf_syms_hdr* header = syms_tag;
    elf_syms_entry* curr_entry = syms_tag + sizeof(elf_syms_hdr);
    printk("    strings %x, size %u\n", header->strTblIdx, header->entryByteSize);
    while ((void *) curr_entry < syms_tag + header->totalSizeBytes) {
        printk("    start %lu, size %lu, nameIdx: %u\n", curr_entry->memAddr, curr_entry->segmentByteSize, curr_entry->nameIdx);
        curr_entry = (void *) curr_entry + header->entryByteSize;
    }
}

static void read_MMAP(void* mmap_tag) {
    mb2_tag_hdr* header = mmap_tag;
    uint16_t size_of_mmap_entry = *(uint16_t*) (mmap_tag + 8);
    mmap_entry* curr_entry = mmap_tag + 16; 
    while ((void*) curr_entry < mmap_tag + header->totalSizeBytes)
    {
        if (curr_entry->type == 1)  // we can use this memory
            printk("*");
        else 
            printk(" ");
        printk("   start %lu, len %lu, ty %u\n", curr_entry->start_addr, curr_entry->byte_len, curr_entry->type);
        curr_entry = (void *) curr_entry + size_of_mmap_entry;
    }
}

void * MMU_pf_alloc() {
    return 0;
}


