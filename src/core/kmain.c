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
    PROC_create_kthread(&read_fat_test, (void *) 0);
    PROC_create_kthread(&md5_test, (void *) 0);
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

int full_tree_cb(char *name, FATDirent *dirent, void *indent) {
    int i;
    if (name[0] == '.')
        return 0;
    if (indent > 0)
        printk("|");
    for (i = 0; i < (uint64_t) indent; i++)
        printk("--");
    if (dirent->attr & FAT_ATTR_DIRECTORY)
        printk("%s/ (size: %u, attr: 0x%x)\n", name, dirent->size, dirent->attr);
    else
        printk("%s (size: %u, attr: 0x%x)\n", name, dirent->size, dirent->attr);
    if ((dirent->attr & FAT_ATTR_DIRECTORY) && name[0] != '.') {
        fat32_readdir((dirent->cluster_hi << 16) + dirent->cluster_lo, full_tree_cb, indent+1);
    }
    return 1;
}

/* run ls on directory given by filepath */
int ls_cb(char *name, FATDirent *dirent, void *filepath){
    FilePath *next = NULL, *curr=NULL;
    if (filepath == NULL) {
        if (dirent->attr & FAT_ATTR_DIRECTORY)
            printk("\n|--%s/ (size: %u, attr: 0x%x)", name, dirent->size, dirent->attr);
        else
            printk("\n|--%s (size: %u, attr: 0x%x)", name, dirent->size, dirent->attr);
        return 1;
    }
    curr = ((FilePath *) filepath);
    if (strcmp(name, curr->name) == 0) {
        next = curr->next;
        printk("%s/", curr->name);
        kfree(curr->name);
        kfree(curr);
        fat32_readdir((dirent->cluster_hi << 16) + dirent->cluster_lo, ls_cb, next);
    }
    return 1;
}

/* read file at filepath */
int read_cb(char *name, FATDirent *dirent, void *filepath){
    FilePath *next = NULL, *curr=NULL;
    File *tgt = NULL;
    char *contents = NULL;
    if (filepath == NULL) {
        return -1;
    }
    curr = ((FilePath *) filepath);

    if (curr->next == NULL) {
        // look for file and read
        if (!(dirent->attr & FAT_ATTR_DIRECTORY) && 
        strcmp(name, curr->name) == 0) {  
            printk("%s: ", curr->name);
            tgt = open(dirent);
            contents = kmalloc(64);
            memset(contents, 0, 64);
            printk("read %d bytes\n", tgt->read(tgt, contents, 63));
            tgt->close(&tgt);
            printk("contents: %s", contents);
        }
    } else if ((dirent->attr & FAT_ATTR_DIRECTORY) && 
                strcmp(name, curr->name) == 0) {
            // traverse directories
            next = curr->next;
            printk("%s/", curr->name);
            fat32_readdir((dirent->cluster_hi << 16) + dirent->cluster_lo, read_cb, next);
    }   
    return 1;
}

void read_fat_test() {
    FilePath *fp;
    fp=split_fpath("hello.txt", '/');
    fat32_readdir(2, read_cb, fp);
    fp=split_fpath("nest/bird.txt", '/');
    fat32_readdir(2, read_cb, fp);
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