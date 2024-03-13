#include "block.h"
#include "kmalloc.h"
#include "asm.h"
#include "print.h"
#include "string.h"

#define PRIM_IO 0x1F0
#define PRIM_CTRL 0x3F6
#define SECOND_IO 0x1F0
#define SECOND_CTRL 0x3F6

#define IDENTIFY 0xEC

// initializes the drive on the primary ata bus and returns the 
// device type if it exists and supports LBA48, ATA_UNKNOWN otherwise.
ata_dev_t ata_block_init(uint16_t io_base, uint16_t ctl_base) {
    uint16_t in_val, mid, hi;
    int i;
    ata_dev_t prim_dev;

    // read the Regular Status byte
    if (inb(PRIM_IO+7) == 0xFF) {
        printk("No disk connected to IDE bus");
        return ATA_UNKNOWN;
    }
    // select a target drive by sending 0xA0 for the master drive
    outb(PRIM_IO+6, 0xA0);
    // set the Sectorcount, LBAlo, LBAmid, and LBAhi IO ports to 0 (port 0x1F2 to 0x1F5)
    outb(PRIM_IO+2, 0);
    outb(PRIM_IO+3, 0);
    outb(PRIM_IO+4, 0);
    outb(PRIM_IO+5, 0);
    // send the IDENTIFY command (0xEC) to the Command IO port (0x1F7)
    outb(PRIM_IO+7, IDENTIFY);
    // read the Status port (0x1F7) again. 
    if ((in_val = inb(PRIM_IO+7)) == 0) {
        printk("Drive does not exist");
        return ATA_UNKNOWN;
    } 
    // poll the Status port (0x1F7) until bit 7 (busy bit, value = 0x80) clears
    while (in_val & 0x80) {
        in_val = inb(PRIM_IO+7);
    }
    if (!((mid = inb(PRIM_IO+4)) | (hi = inb(PRIM_IO+5)))) {
        // poll one of the Status ports until bit 3 or 8 sets.
        while (!(in_val & 0x09)) {
            in_val = inb(PRIM_IO+7);
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
        in_val = inw(PRIM_IO);
        // uint16_t 83: Bit 10 is set if the drive supports LBA48 mode.
        if (i == 83 && (in_val & (1 << 10))) {
            dprintk(DPRINT_DETAILED, "Supports LBA48\n");
            return prim_dev;
        } else if (i == 83){
            dprintk(DPRINT_DETAILED, "%x Doesn't support LBA48\n", in_val);
        }
    } 
    return ATA_UNKNOWN;
}
/* 
check:
   - the controller is present, 
   - supports 48-but LBA mode, 
   - the API mode used by the device (ATA, ATAPI, SATA, or SATAPI). 
*/
ATABlockDev *ata_probe(
    uint16_t io_base, 
    uint16_t ctl_base,
    uint8_t isMaster, 
    const char *name, 
    uint8_t irq
) {
    ATABlockDev *ata = NULL;
    ata_dev_t dev = ata_block_init(io_base, ctl_base);

    if (dev == ATA_PATA) {
        ata = kmalloc(sizeof(*ata));
        memset(ata, 0, sizeof(*ata));
        ata->io_base = io_base;
        ata->ctl_base = ctl_base;
        ata->isMaster = isMaster;
        ata->irq = irq; // which irq num to take
        //ata->dev.read_block = &ata_48_read_block;
    } else {
        printk("unsupported device");
    }
    return ata;
}