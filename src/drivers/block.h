#ifndef BLOCK_H
#define BLOCK_H
#include <stdint.h>

enum BlockDevType { MASS_STORAGE, PARTITION };

typedef enum {
	ATA_PATA,
   ATA_SATA,
   ATA_PATAPI,
   ATA_SATAPI,
   ATA_UNKNOWN
} ata_dev_t;

typedef struct BlockDev {
   uint64_t tot_length;
   int (*read_block)(struct BlockDev *dev, uint64_t blk_num, void *dst);
   uint32_t blk_size;
   enum BlockDevType type;
   const char *name;
   uint8_t fs_type;
   struct BlockDev *next;
} BlockDev;

/* ATA inherits from BlockDev */
typedef struct ATABlockDev {
   struct BlockDev dev;
   uint16_t io_base, ctl_base;
   uint8_t isMaster;
   uint16_t irq;
   struct ATARequest *req_head, *req_tail;
} ATABlockDev;


ATABlockDev *ata_probe(uint16_t base, uint16_t master,
          uint8_t slave, const char *name, uint8_t irq);
ata_dev_t ata_block_init(uint16_t io_base, uint16_t ctl_base);
#endif