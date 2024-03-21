#include "block.h"
#include "kmalloc.h"
#include "asm.h"
#include "print.h"
#include "string.h"
#include "proc.h"

#define PRIM_IO 0x1F0
#define PRIM_CTRL 0x3F6
#define SECOND_IO 0x1F0
#define SECOND_CTRL 0x3F6

#define IDENTIFY 0xEC
#define READ_SECTORS 0x24

typedef struct BDNode {
    BlockDev *curr;
    struct BDNode *next, *prev;
} BDNode;

static BDNode *node_list = NULL;
static thread_q *bd_block_q;

int ata_48_read_block(BlockDev *dev, uint64_t blk_num, void *dst);

// initializes the drive on the primary ata bus and returns the 
// device type if it exists and supports LBA48, ATA_UNKNOWN otherwise.
ata_dev_t ata_block_init(uint16_t io_base, uint16_t ctl_base, uint64_t *lba_48_sector_ct) {
    uint16_t in_val, mid, hi;
    int i;
    ata_dev_t prim_dev;

    // reset drive 
    outb(io_base, 1 << 2);
    for (i=0; i<15; i++) {
        // wait
        in_val = inb(io_base+7);
    }
    outb(io_base, 0);

    // read the Regular Status byte
    if (inb(io_base+7) == 0xFF) {
        printk("No disk connected to IDE bus");
        return ATA_UNKNOWN;
    }
    // select a target drive by sending 0xA0 for the master drive
    outb(io_base+6, 0xA0);
    for (i=0; i<15; i++) {
        // wait
        in_val = inb(io_base+7);
    }
    // set the Sectorcount, LBAlo, LBAmid, and LBAhi IO ports to 0 (port 0x1F2 to 0x1F5)
    outb(io_base+2, 0);
    outb(io_base+3, 0);
    outb(io_base+4, 0);
    outb(io_base+5, 0);
    // send the IDENTIFY command (0xEC) to the Command IO port (0x1F7)
    outb(io_base+7, IDENTIFY);
    // read the Status port (0x1F7) again. 
    if ((in_val = inb(io_base+7)) == 0) {
        printk("Drive does not exist");
        return ATA_UNKNOWN;
    } 
    // poll the Status port (0x1F7) until bit 7 (busy bit, value = 0x80) clears
    while (in_val & 0x80) {
        printk("polling1...\n");
        in_val = inb(io_base+7);
    }
    if (!((mid = inb(io_base+4)) || (hi = inb(io_base+5)))) {
        // poll one of the Status ports until bit 3 or 8 sets.
        while (!(in_val & 0x09)) {
            in_val = inb(io_base+7);
            printk("polling2...\n");
        }
        dprintk(DPRINT_DETAILED, "PATA\n");
        prim_dev = ATA_PATA;
    } else if (mid == 0x14 && hi == 0xEB) {
        dprintk(DPRINT_DETAILED, "PATAPI\n");
        prim_dev = ATA_PATAPI;
    } else if (mid == 0x69 && hi == 0x96) {
        dprintk(DPRINT_DETAILED, "SATAPI\n");
        prim_dev = ATA_SATAPI;
    } else if (mid == 0x3c && hi == 0xc3) {
        dprintk(DPRINT_DETAILED, "SATA\n");
        prim_dev = ATA_SATA;
    } else {
        dprintk(DPRINT_DETAILED, "Unknown device type.\n");
        return ATA_UNKNOWN;
    }

    // read 256 16-bit values from IDENTIFY
    for (i = 0; i < 256; i++) {
        in_val = inw(io_base);
        // uint16_t 83: Bit 10 is set if the drive supports LBA48 mode.
        if (i == 83 && (in_val & (1 << 10))) {
            dprintk(DPRINT_DETAILED, "Supports LBA48\n");
        } else if (i == 83){
            dprintk(DPRINT_DETAILED, "%x Doesn't support LBA48\n", in_val);
        }
        if (i >= 100 && i <= 103) {
            *lba_48_sector_ct = *lba_48_sector_ct + (in_val << 16 * (i-100));
        }
    } 
    dprintk(DPRINT_DETAILED, "lba sector_ct: %lx\n", *lba_48_sector_ct);
    return prim_dev;
}
/* 
check:
   - the controller is present, 
   - supports 48-but LBA mode, 
   - the API mode used by the device (ATA, ATAPI, SATA, or SATAPI). 
if so, register with driver under given name
else, return NULL
*/
ATABlockDev *ata_probe(
    uint16_t io_base, 
    uint16_t ctl_base,
    uint8_t isMaster, 
    const char *name, 
    uint8_t irq
) {
    ATABlockDev *ata = NULL;
    uint64_t lba_48_sector_ct = 0;
    ata_dev_t dev = ata_block_init(io_base, ctl_base, &lba_48_sector_ct);

    if (dev == ATA_PATA) {
        ata = kmalloc(sizeof(*ata));
        memset(ata, 0, sizeof(*ata));
        ata->io_base = io_base;
        ata->ctl_base = ctl_base;
        ata->isMaster = isMaster;
        ata->irq = irq; // which irq num to take
        ata->dev.read_block = &ata_48_read_block;
        ata->dev.type = dev;
        ata->dev.tot_length = lba_48_sector_ct;
        ata->dev.name = name;

        BLK_register_dev((BlockDev *) ata);
    } else {
        printk("unsupported device");
    }
    return ata;
}

// TODO: add block offset device

/* read block from ata device, assumes dev is an ata device */
int ata_48_read_block(BlockDev *dev, uint64_t blk_num, void *dst) {
    int i;
    uint16_t io_base;
    uint16_t num_sectors=1;
    if (dev == NULL) {
        return 1;
    }
    io_base = ((ATABlockDev *) dev)->io_base;
    //dev->blk_size
    outb(io_base+6, 0x40); // 0x40 for the "master" or 0x50 for the "slave"
    outb(io_base+2, num_sectors >> 8);   // sector hi
    outb(io_base+3, (blk_num >> 24) & 0xFF); // byte 4
    outb(io_base+4, (blk_num >> 32) & 0xFF); // byte 5
    outb(io_base+5, (blk_num >> 40) & 0xFF); // byte 6
    outb(io_base+2, num_sectors & 0xFF); // sector lo
    outb(io_base+3, blk_num & 0xFF);         // byte 1
    outb(io_base+4, blk_num >> 8 & 0xFF);    // byte 2
    outb(io_base+5, blk_num >> 16 & 0xFF);   // byte 3
    outb(io_base+7, READ_SECTORS);

    cli();
    // block and wait for interrupt
    if (bd_block_q == NULL) {
        bd_block_q = kmalloc(sizeof(thread_q));
        memset(bd_block_q, 0, sizeof(thread_q));
    }
    PROC_block_on(bd_block_q, 0);
    sti();

    for (i = 0; i < 256; i++) {
        *(((uint16_t *) dst) + i) = inw(io_base);
    } 
    return 0;
}

void ata_irq() {
    // only support primary io bus for now
    // TODO: actually use the node_list 
    if (inb(PRIM_IO+7) & 1) {
        printk("ATA error %x", inw(PRIM_IO+1));
        return;
    }
    if (bd_block_q != NULL) {
        PROC_unblock_head(bd_block_q);
    }
}

int BLK_register_dev(struct BlockDev *dev) {
    BDNode *new_node = kmalloc(sizeof(BDNode));
    new_node->curr = dev;
    new_node->next = node_list;
    node_list = new_node;
    return 0;
}

/*
returns block device with name or NULL if not exists
*/
BlockDev *get_block_device(char *name) {
    BDNode *node = node_list;
    if (node_list == NULL) {
        printk("No block devices created\n");
    }
    while(node != NULL) {
        if (strcmp(name, node->curr->name) == 0) {
            return node->curr;
        }
        node = node->next;
    }
    printk("No block devices with name \"%s\" found\n", name);
    return NULL;
}