#include "print.h"
#include "ps2.h"
#include "interrupts.h"
#include "serial.h"
#include "page_alloc.h"
#include "asm.h"
#include "string.h"
#include "page_alloc.h"
#include "kmalloc.h"
#include <stdint.h>
#include <limits.h>
void fill_page_with_hash(void* addr, uint64_t page_size) ;
int check_page_hash(void* addr, uint64_t page_size) ;
void test_pf_alloc();

#define HW "hello world :)"
#define LONGTEXT "howdy the aim of the game is to try and fill up a lot of text!!!!"
#define LONGERTEXT "howdy the aim of the game is to try and fill up a lot more text!!!! hahahahahahahahahahahahahahahahahahahahahahahahahaha :) neato :^) ....."
#define NEATO "neato"
void test_vpage_alloc();
void testmalloc();
void *mallocFill(uint64_t sizeB, char val);
int checkFill(void * addr, uint64_t sizeB, char val);

/* kernel main function */
int kmain(uint64_t rbx) {
    // int gdb = 1;
    // while(gdb);
    void *mbr_ptr = (void*) (rbx & 0xFFFFFFFF);
    //uint64_t *vptr = (uint64_t *) 0xB0BBEEFF000;
    disable_interrupts();
    init_ps2();
    enable_interrupts();
    MMU_init(mbr_ptr);

    while(1);
    return 0;
}

void testmalloc() {
    uint64_t i = 0, fill_size, old_sz;
    char fill_val, old_val;
    void *old = 0, *curr;
    
    kmalloc_set_babble(1);

    for (i = 0; i < 10000; i++) {
        fill_size = (i % 2000) * 3 + 1;
        fill_val = (i % 90) + 33;
        curr = mallocFill(fill_size, fill_val);
        if (i % 11 == 2) {
            if (old != 0) {
                checkFill(old, old_sz, old_val);
                kfree(old);
            }
            old = curr;
            old_val = fill_val;
            old_sz = fill_size;
        }
    }
    
    kmalloc_set_babble(0);

    printk("malloc test success");
}

void test_vpage_alloc() {
    int pg_num;
    void *curr_page = (void *)0xBEEFCAFE000, 
    *old_page = (void *) 0;
    pg_num = 0;
    while(pg_num < 10000) {
        curr_page = vpage_alloc((uint64_t) curr_page);
        printk("%p\n", curr_page);
        fill_page_with_hash(curr_page, 4096);
        check_page_hash(curr_page, 4096);
        
        // every 7, free old page and update old_page to curr_page
        if (pg_num % 7 == 0) {
            if (old_page != 0) {
                check_page_hash(old_page, 4096);
                vpage_free((uint64_t) old_page);
            }
            old_page = curr_page;
        }
        pg_num++;
        curr_page += 4096 * 7;
    } 
}

/* mallocs a ptr of sizeB and fills with val */
void *mallocFill(uint64_t sizeB, char val) {
    void *ans = kmalloc(sizeB);
    memset(ans, val, sizeB);
    return ans;
}

int checkFill(void * addr, uint64_t sizeB, char val) {
    int i;
    for (i=0; i<sizeB; i++) {
        if (*(char *) (addr + i) != val) {
            printk("test fail");
            return 0;
        }
    }
    return 1;
}

void test_pf_alloc() {
    int pg_num;
    void *curr_page, *old_page = 0;
    pg_num = 0;
    while((curr_page = MMU_pf_alloc())) {
        printk("%p\n", curr_page);
        fill_page_with_hash(curr_page, 4096);
        check_page_hash(curr_page, 4096);
        // check that our old page is still good
        if (old_page != 0) {
            check_page_hash(old_page, 4096);
        }
        // every 7, kfree old page and update old_page to curr_page
        if (pg_num % 7 == 0) {
            MMU_pf_free(old_page);
            old_page = curr_page;
        }
        pg_num++;
    } 
}

void fill_page_with_hash(void* addr, uint64_t page_size) {
    // fills the location [addr, addr+num_bytes) with
    // a repeating pattern that is a hash of val
    char hash = ((uint64_t)addr / page_size) % 64 + 33;
    memset(addr, hash, page_size);
}
int check_page_hash(void* addr, uint64_t page_size) {
    char hash = ((uint64_t)addr / page_size) % 64 + 33;    
    int i;
    for (i=0; i<page_size; i++) {
        if (*(char *) (addr + i) != hash) {
            printk("test fail");
            return 0;
        }
    }
    return 1;
}