#include <stdint.h>
#include <stddef.h>
#include "block.h"
#include "string.h"
#include "kmalloc.h"
#include "print.h"
#include "mbr.h"
#include "fat32.h"

#define DIRENT_SIZE 32
#define BLK_DEV_NAME "fat32_device"
#define FAT_ATTR_READ_ONLY 0x01
#define FAT_ATTR_HIDDEN 0x02
#define FAT_ATTR_SYSTEM 0x04
#define FAT_ATTR_VOLUME_ID 0x08
#define FAT_ATTR_DIRECTORY 0x10
#define FAT_ATTR_ARCHIVE 0x20
#define FAT_ATTR_LFN (FAT_ATTR_READ_ONLY | FAT_ATTR_HIDDEN | \
                    FAT_ATTR_SYSTEM | FAT_ATTR_VOLUME_ID)

typedef struct FATDirent {
    char name[11];
    uint8_t attr;
    uint8_t nt;
    uint8_t ct_tenths;
    uint16_t ct;
    uint16_t cd;
    uint16_t ad;
    uint16_t cluster_hi;
    uint16_t mt;
    uint16_t md;
    uint16_t cluster_lo;
    uint32_t size;
} __attribute__((packed)) FATDirent;

typedef struct FATLongDirent {
    uint8_t order;
    uint16_t first[5];
    uint8_t attr;
    uint8_t type;
    uint8_t checksum;
    uint16_t middle[6];
    uint16_t zero;
    uint16_t last[2];
} __attribute__((packed)) FATLongDirent;

static int initialized = 0;
static mbr_table *mbr = NULL;
FAT32 *fat = NULL;

void print_fat32(FAT32 *fat);
FAT32 *read_fat32(uint64_t block_num, char *dev_name);
// TODO: use fat
// read through a path

/* try to initialize device 1 on success, 0 on failure */
int fat32_init() {
    if (ata_probe(0x1F0, 0x3F6, 1, BLK_DEV_NAME, 0x2E) != NULL) {
        mbr = read_mbr(BLK_DEV_NAME);
        fat = read_fat32(mbr->pe_1.first_lba, BLK_DEV_NAME);
        // TODO map entire fat table to memory      
        initialized = 1;
    }
    return initialized;
}

void read_fat32_dirent() {
    uint64_t cluster_2_sector;
    uint8_t *buf = NULL;
    FATDirent *dirent = NULL;
    FATLongDirent *ldirent = NULL;
    ATABlockDev *d = NULL;
    if (initialized || fat32_init()) {
        // locate dirent
        cluster_2_sector = mbr->pe_1.first_lba + fat->bpb.reserved_sectors + fat->sectors_per_fat * fat->bpb.num_fats;
        
        // read root dirent
        buf = (uint8_t *) kmalloc(512);
        
        d = (ATABlockDev *) get_block_device(BLK_DEV_NAME);
        d->dev.read_block((BlockDev *) d, cluster_2_sector, buf);
        if (buf[11] == FAT_ATTR_LFN) {
            ldirent = kmalloc(sizeof(FATLongDirent));
            memcpy(ldirent, buf, DIRENT_SIZE);
            printk("LFN name: %c%c%c%c%c\n", ldirent->first[0], ldirent->first[1], ldirent->first[2], ldirent->first[3], ldirent->first[4]);
            kfree(ldirent);
        } else {
            dirent = kmalloc(sizeof(FATDirent));
            memcpy(dirent, buf, DIRENT_SIZE);
            printk("ent name: %c%c%c, size: 0x%x\n", dirent->name[0],dirent->name[1],dirent->name[2], dirent->size);
            printk("flags: 0x%x\n", dirent->attr);
            kfree(dirent);
        }
        kfree(buf);
    }
}

FAT32 *read_fat32(uint64_t block_num, char *dev_name) {
    ATABlockDev *d = NULL;
    FAT32 *fat32 = NULL;
    uint8_t *buf = NULL;
    buf = (uint8_t *) kmalloc(512);
    d = (ATABlockDev *) get_block_device(dev_name);
    fat32 = (FAT32 *) kmalloc(sizeof(FAT32));
    d->dev.read_block((BlockDev *) d, block_num, buf);
    
    memcpy(fat32, buf, sizeof(FAT32));
    if (!(fat32->signature == 0x28 || fat32->signature == 0x29) ||
    fat32->boot_sig[0] != 0x55 || fat32->boot_sig[1] != 0xAA)  {
        printk("read_fat32 did not find the correct signatures\n");
        kfree(fat32);
        fat32 = NULL;
    }
    kfree(buf);
    print_fat32(fat32);
    return fat32;
}

void print_fat32(FAT32 *fat) {
    printk("bytes per sec: %d, sec per cluster: %d\n", fat->bpb.bytes_per_sector, fat->bpb.sectors_per_cluster);
    printk("num fats %d, sec per fat %d\n", fat->bpb.num_fats, fat->sectors_per_fat);
    printk("root cluster: 0x%x\n", fat->root_cluster_number);
}

