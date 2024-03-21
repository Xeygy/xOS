#ifndef FAT32_H
#define FAT32_H
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

void read_fat32_dirent();
#endif