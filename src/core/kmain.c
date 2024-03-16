#include "print.h"
#include "ps2.h"
#include "interrupts.h"
#include "page_alloc.h"
#include "proc.h"
#include "snakes.h"
#include "syscall.h"
#include "block.h"
#include "asm.h"
#include "kmalloc.h"
#include <stdint.h>

void test_threads(void* arg);
void keyboard_io(void *);
void read_blk_test();

/* kernel main function */
int kmain(uint64_t rbx) {
    // int gdb = 1;
    // while(gdb);
    void *mbr_ptr = (void*) (rbx & 0xFFFFFFFF);
    set_debug_verbosity(DPRINT_DETAILED);
    firstTimeSetup();
    MMU_init(mbr_ptr);
    cli();
    init_ps2();
    sti();
    init_proc();
    // syscall(SYS_TEST);
    PROC_create_kthread(&read_blk_test, (void *) 0);
    PROC_create_kthread(&keyboard_io, (void *) 0);
    //PROC_create_kthread(&test_threads, (void *) 8);
    //setup_snakes(1);
    while (1) {
        PROC_run();
        asm volatile ("hlt");
    }
}

void read_blk_test() {
    ATABlockDev *d;
    int i;
    uint8_t *blk = kmalloc(sizeof(uint16_t)*256);
    d = ata_probe(0x1F0, 0x3F6, 1, "main", 1);
    d->dev.read_block((BlockDev *)d, 32, blk);
    for (i = 0; i < 512; i++) {
        printk("%x", blk[i]);
    }
}

void test_threads(void* arg) {
    int i = 0;
    for (i = 1; i <= (uint64_t) arg; i++) {
        p_int();
        printk("%d) I am thread %p\n", i, arg);
        dprintk(DPRINT_NORMAL, "you see me");
        yield();    
    }
}

void keyboard_io(void *) {
   while (1)
      printk("%c", getc());
}