#include <stdint.h>
#include <stddef.h>
#include "mbr.h"
#include "block.h"
#include "string.h"
#include "print.h"
#include "kmalloc.h"

#define BOOT_CODE_LEN 446

static void print_mbr_ent(mbr_entry *entry, char *prefix);

mbr_table *read_mbr() {
    ATABlockDev *d = NULL;
    uint8_t *buf = NULL;
    mbr_table *mbr_tb =NULL;
    d = (ATABlockDev *) get_block_device("ata_main");
    if (d == NULL) {
        return NULL;
    }
    buf = (uint8_t *) kmalloc(512);
    mbr_tb = (mbr_table *) kmalloc(sizeof(mbr_table));
    d->dev.read_block((BlockDev *) d, 0, buf);
    memcpy(mbr_tb, buf + BOOT_CODE_LEN, sizeof(mbr_table));
    if (mbr_tb->sig_1 != 0x55 || mbr_tb->sig_2 != 0xAA) {
        printk("read_mbr did not find the correct signatures\n");
        kfree(mbr_tb);
        mbr_tb = NULL;
    }
    kfree(buf);
    return mbr_tb;
}

void print_mbr(mbr_table *mbr) {
    if (mbr == NULL) {
        return;
    }
    printk("** MBR **\n");
    printk("Entry 1\n");
    print_mbr_ent(&mbr->pe_1, "|-- ");
    printk("Entry 2\n");
    print_mbr_ent(&mbr->pe_2, "|-- ");
    printk("Entry 3\n");
    print_mbr_ent(&mbr->pe_3, "|-- ");
    printk("Entry 4\n");
    print_mbr_ent(&mbr->pe_4, "|-- ");
    printk("Signature 0x%x 0x%x\n", mbr->sig_1, mbr->sig_2);
}

static void print_mbr_ent(mbr_entry *entry, char *prefix) {
    if (entry == NULL) {
        return;
    }
    printk("%sstatus:    0x%x\n", prefix, entry->status);
    printk("%stype:      0x%x\n", prefix, entry->part_type);
    printk("%sLBA:       %d\n", prefix, entry->first_lba);
    printk("%ssector ct: %d\n", prefix, entry->sector_ct);
    // 30720 - 2048 = 28672, why 28673? is end inclusive?
    // https://www.gnu.org/software/parted/manual/html_node/mkpart.html 
}