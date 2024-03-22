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
#include "mbr.h"
#include "fat32.h"
#include "f32_interface.h"
#include "fs_utils.h"
#include "string.h"
#include "md5.h"
#include <stdint.h>

void test_threads(void* arg);
void keyboard_io(void *);
void read_blk_test();
void read_mbr_test();
void read_fat_test();
void dirsplit_test();
void str_builder_test();
void md5_test();
void md5_on_new_files();

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
    //PROC_create_kthread(&read_blk_test, (void *) 0);
    //PROC_create_kthread(&read_fat_test, (void *) 0);
    PROC_create_kthread(&md5_on_new_files, (void *) 0);
    // PROC_create_kthread(&str_builder_test, (void *) 0);
    PROC_create_kthread(&keyboard_io, (void *) 0);
    //PROC_create_kthread(&test_threads, (void *) 8);
    //setup_snakes(1);
    while (1) {
        PROC_run();
        asm volatile ("hlt");
    }
}

void md5_test() {
    uint8_t hash[16];
    int i;
    md5String("", hash);
    for (i = 0; i<16; i++) {
        if (hash[i] < 0x10)
            printk("0");
        printk("%x", hash[i]);
    }
    printk("\nd41d8cd98f00b204e9800998ecf8427e\n");

    md5String("message digest", hash);
    for (i = 0; i<16; i++) {
        if (hash[i] < 0x10)
            printk("0");
        printk("%x", hash[i]);
    }
    printk("\nf96b697d7cb7938d525a2f31aaf161d0\n");
} 

void read_mbr_test() {
    mbr_table *mbr = read_mbr("ata_main");
    print_mbr(mbr);
    kfree(mbr);
}

void dirsplit_test() {
    FilePath * fp;
    fp = split_fpath("//hello/how/are//you//", '/');
    while (fp != NULL) {
        printk("%s :: ", fp->name);
        fp = fp->next;
    }
}

void str_builder_test() {
    StringBuilder *nolan = NULL;
    char *full = NULL;
    nolan = insert_sb(nolan, "name ", 1);
    nolan = insert_sb(nolan, "ive ", 4);
    nolan = insert_sb(nolan, "my ", 0);
    nolan = insert_sb(nolan, "is ", 2);
    nolan = insert_sb(nolan, "please.", 6);
    nolan = insert_sb(nolan, "i want to sell computers to you, ", 5);
    nolan = insert_sb(nolan, "jony ", 3);
    full = build_string(nolan);
    printk("\n%s\n", full);
}

void read_fat_test() {
    f32_print_contents("hello.txt", 32);
    f32_print_contents("nonexistant.txt", 32);
    f32_print_contents("nest/bird.txt", 32);
}

void md5_on_new_files() {
    printk("\nhello: ");
    f32_md5("hello.txt");
    printk("\nacpi:  ");
    f32_md5("boot/grub/i386-pc/acpi.mod");
    printk("\nminix: ");
    f32_md5("boot/grub/i386-pc/minix.mod");
}

void read_blk_test() {
    ATABlockDev *d;
    int i;
    uint8_t *blk = kmalloc(sizeof(uint16_t)*256);
    // TODO block until read a specific block w/map
    d = (ATABlockDev *) get_block_device("ata_main");
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