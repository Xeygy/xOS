#ifndef FAT32_H
#define FAT32_H

#include <stdint.h>

#define FAT_ATTR_READ_ONLY 0x01
#define FAT_ATTR_HIDDEN 0x02
#define FAT_ATTR_SYSTEM 0x04
#define FAT_ATTR_VOLUME_ID 0x08
#define FAT_ATTR_DIRECTORY 0x10
#define FAT_ATTR_ARCHIVE 0x20
#define FAT_ATTR_LFN (FAT_ATTR_READ_ONLY | FAT_ATTR_HIDDEN | \
                    FAT_ATTR_SYSTEM | FAT_ATTR_VOLUME_ID)

#define FAT_ROOT_CLUSTER 2

typedef struct FAT_BPB {
    uint8_t jmp[3];
    char    oem_id[8];
    uint16_t bytes_per_sector;  // imp
    uint8_t sectors_per_cluster;  // imp
    uint16_t reserved_sectors;
    uint8_t num_fats;  // imp
    uint16_t num_dirents;
    uint16_t tot_sectors;
    uint8_t mdt;
    uint16_t num_sectors_per_fat;  // imp
    uint16_t sectors_per_track;
    uint16_t num_heads;
    uint32_t num_hidden_sectors;
    uint32_t large_sector_count;
} __attribute__((packed)) FAT_BPB;

typedef struct FAT32 {
    FAT_BPB bpb;
    uint32_t sectors_per_fat;
    uint16_t flags;
    uint8_t major_vers;
    uint8_t minor_vers;
    uint32_t root_cluster_number; // reserved_sectors + sectors_per_fat * num_fats.
    uint16_t fsinfo_sector;
    uint16_t backup_boot_sector;
    uint8_t zero[12];
    uint8_t drive_num;
    uint8_t nt_flags;
    uint8_t signature;
    uint32_t serial_num;
    char label[11];
    char sys_id[8];
    uint8_t boot_code[420];
    uint8_t boot_sig[2];
} __attribute__((packed)) FAT32;

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

typedef struct File {
    uint8_t			f_mode; // Read/Write perms
	uint64_t		f_pos;  // read offset
    FATDirent       *dirent; // metadata
    // TODO: eventually put into f_op
    int (*close) (struct File **file);
    int (*read) (struct File *file, char *dst, int len);
    // int (*write)(struct File *file, char *dst, int len);
    int (*lseek) (struct File *file, uint64_t offset);
} File;

/* 
callback for fat32_readdir
takes the current directory's name, fatdirent metadata, and optional params
*/
typedef int (*fat32_readdir_cb)(char *, FATDirent *, void *);

/* read directory at cluster_number and apply callback with arg p to dirents */
void fat32_readdir(uint64_t cluster_number, fat32_readdir_cb cb, void *p);
/* returns file at dirent*/
File *open(FATDirent *dirent);

#endif