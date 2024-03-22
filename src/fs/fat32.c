#include <stdint.h>
#include <stddef.h>
#include "block.h"
#include "string.h"
#include "kmalloc.h"
#include "print.h"
#include "mbr.h"
#include "fs_utils.h"
#include "fat32.h"

#define DIRENT_SIZE 32
#define BLK_DEV_NAME "fat32_device"
#define SKIP_DIRENT 0xE5
#define FMODE_RD 0x01
#define FMODE_WR 0x02

static int initialized = 0;
static mbr_table *mbr = NULL;
FAT32 *fat = NULL;

void print_fat32(FAT32 *fat);
FAT32 *read_fat32(uint64_t block_num, char *dev_name);
uint64_t next_in_chain(uint64_t fat_idx_start);
int fat32_read(File *f, char *dst, int len);
int fat32_close(File **fp);
int fat32_lseek(struct File *file, uint64_t offset);

/* try to initialize device 1 on success, 0 on failure */
int fat32_init() {
    if (ata_probe(0x1F0, 0x3F6, 1, BLK_DEV_NAME, 0x2E) != NULL) {
        mbr = read_mbr(BLK_DEV_NAME);
        fat = read_fat32(mbr->pe_1.first_lba, BLK_DEV_NAME);
        initialized = 1;
    }
    return initialized;
}

void fat32_readdir(uint64_t cluster_number, fat32_readdir_cb cb, void *p) {
    uint64_t cluster_2_lba, tgt_lba, i;
    uint8_t *buf = NULL;
    FATDirent *dirent = NULL;
    FATLongDirent *ldirent = NULL;
    ATABlockDev *d = NULL;
    StringBuilder *sb = NULL;
    char *segment = NULL;
    int end = 0;

    if (cluster_number < 2) {
        printk("read_fat32_directory: cluster number must be >= 2, got %lu\n", cluster_number);
        return;
    }
    if (!initialized) {
        if (!fat32_init()) 
            return;
    }

    cluster_2_lba = mbr->pe_1.first_lba + fat->bpb.reserved_sectors + fat->sectors_per_fat * fat->bpb.num_fats;
    
    // read cluster
    buf = (uint8_t *) kmalloc(512);
    d = (ATABlockDev *) get_block_device(BLK_DEV_NAME);

    do {
        tgt_lba = cluster_2_lba + (cluster_number - 2) * fat->bpb.sectors_per_cluster;
        d->dev.read_block((BlockDev *) d, tgt_lba, buf);

        for (i = 0; (i < 512) && !end; i += DIRENT_SIZE) {
            if (buf[i+11] == FAT_ATTR_LFN) {
                ldirent = kmalloc(sizeof(FATLongDirent));
                memcpy(ldirent, buf+i, DIRENT_SIZE);

                segment = read_lfn_str(ldirent);
                sb = insert_sb(sb, segment, ldirent->order & 0x3F); // mask to get order
                kfree(segment);
                kfree(ldirent);
            } else {
                dirent = kmalloc(sizeof(FATDirent));
                memcpy(dirent, buf+i, DIRENT_SIZE);

                if (dirent->name[0] == 0) {
                    // no more entries in this directory
                    end = 1;
                } else if (dirent->name[0] == SKIP_DIRENT) {
                    // skip the current dirent
                    if (sb != NULL)
                        kfree(build_string(sb)); // hack to free string builder
                } else {
                    if (sb != NULL) {
                        segment = build_string(sb);
                    } else {
                        segment = read_classic_dir_str(dirent);
                    }
                    // run the callback on the current dirent
                    if (cb != NULL)
                        cb(segment, dirent, p);
                    kfree(segment);
                }
                sb = NULL;
                kfree(dirent);
            }
        }
        cluster_number = next_in_chain(cluster_number);
    } while (!end && cluster_number != 0);
    if (!end) {
        printk("shouldn't get here, incomplete cluster read\n");
    }
    kfree(buf);
}

// returns the value of the next in chain or 0 if last entry
uint64_t next_in_chain(uint64_t fat_idx_start) {
    uint64_t tgt_block, tgt_offset;
    uint32_t entry;
    uint8_t *buf = NULL;
    ATABlockDev *d = NULL;

    if (fat_idx_start/(fat->bpb.bytes_per_sector/4) >= fat->sectors_per_fat) {
        printk("read_cluster_chain: fat_idx too high, index out of bounds in FAT\n");
        printk("..start %lu, bps: %u, spf: %u\n", fat_idx_start, fat->bpb.bytes_per_sector, fat->sectors_per_fat);
        return 0;
    }

    if (fat->bpb.bytes_per_sector != 512 || fat->bpb.sectors_per_cluster != 1) {
        printk("read_cluster_chain: unexpected bytes_per_sector and/or sectors_per_cluster\n");
    }
    // index into fat table, assume bytes_per_sector = 512, sectors_per_cluster = 1, block size 512
    tgt_block = fat_idx_start/(512/4) + mbr->pe_1.first_lba + fat->bpb.reserved_sectors;
    tgt_offset = fat_idx_start % (512/4);

    buf = (uint8_t *) kmalloc(512);
    d = (ATABlockDev *) get_block_device(BLK_DEV_NAME);
    d->dev.read_block((BlockDev *) d, tgt_block, buf);
    entry = *((uint32_t*) &buf[tgt_offset * 4]) & 0x0FFFFFFF;
    kfree(buf);
    if (entry >= 0x0FFFFFF8) {
        return 0;
    }    
    return entry;
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
    return fat32;
}

void print_fat32(FAT32 *fat) {
    printk("bytes per sec: %d, sec per cluster: %d\n", fat->bpb.bytes_per_sector, fat->bpb.sectors_per_cluster);
    printk("num fats %d, sec per fat %d\n", fat->bpb.num_fats, fat->sectors_per_fat);
    printk("root cluster: 0x%x\n", fat->root_cluster_number);
}

// open file at dirent
File *open(FATDirent *dirent) {
    File *newf = NULL;
    FATDirent *dirent_copy = NULL;
    if (dirent != NULL) {
        newf = kmalloc(sizeof(File));
        dirent_copy = kmalloc(sizeof(FATDirent));
        memcpy(dirent_copy, dirent, sizeof(FATDirent));

        newf->f_mode = FMODE_RD;
        newf->f_pos = 0;
        newf->dirent = dirent_copy;
        newf->read = fat32_read;
        newf->close = fat32_close;
        newf->lseek = fat32_lseek;
    }
    return newf;
} 

/* frees the file pointed to and sets the pointer to NULL */
int fat32_close(File **fp) {
    if (*fp == NULL) {
        printk("Tried to close a NULL file pointer");
        return -1;
    }
    kfree(*fp);
    *fp = NULL;
    return 0;
}

/* 
read len from file into dst, return num bytes transfered,
-1 on error.
*/
int fat32_read(File *f, char *dst, int len) {
    ATABlockDev *d = NULL;
    uint64_t bytes_to_read, bytes_from_block, bytes_read, 
    clusters_to_skip, cluster, cluster_2_lba, tgt_lba, tgt_offset;
    int i;
    uint8_t *buf = NULL;
    if (!initialized) {
        if (!fat32_init()) 
            return -1;
    }

    buf = (uint8_t *) kmalloc(512);
    d = (ATABlockDev *) get_block_device(BLK_DEV_NAME);
    bytes_read = 0;
    cluster = (f->dirent->cluster_hi << 16) + f->dirent->cluster_lo;
    cluster_2_lba = mbr->pe_1.first_lba + fat->bpb.reserved_sectors + fat->sectors_per_fat * fat->bpb.num_fats;
    if (f->dirent->size - f->f_pos < len) {
        bytes_to_read = f->dirent->size - f->f_pos;
    } else {
        bytes_to_read = len;
    }

    clusters_to_skip = f->f_pos/512; // cluster size
    for (i=0; i < clusters_to_skip; i++)
        cluster = next_in_chain(cluster);

    do {
        tgt_lba = cluster_2_lba + (cluster - 2) * fat->bpb.sectors_per_cluster;
        tgt_offset = f->f_pos % 512;
        d->dev.read_block((BlockDev *) d, tgt_lba, buf); 
        if (512-tgt_offset < bytes_to_read) {
            bytes_from_block = 512-tgt_offset;
        } else {
            bytes_from_block = bytes_to_read;
        }
        memcpy(dst + bytes_read, buf+tgt_offset, bytes_from_block);
        bytes_read += bytes_from_block;
        
        cluster = next_in_chain(cluster);
        tgt_offset = 0;
        bytes_to_read -= bytes_from_block;
        f->f_pos += bytes_from_block;
    } while (cluster != 0);

    kfree(buf);
    return bytes_read;
}

/* sets the file's read offset to offset, returns new offset */
int fat32_lseek(struct File *file, uint64_t offset) {
    if (file == NULL) {
        printk("Tried to close a NULL file pointer");
        return -1;
    }
    file->f_pos = offset;
    return offset;
}