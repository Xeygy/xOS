#include "print.h"
#include "ps2.h"
#include "interrupts.h"
#include "serial.h"
#include "page_alloc.h"
#include "asm.h"
#include "string.h"
#include "page_alloc.h"
#include <stdint.h>
#include <limits.h>
void fill_page_with_hash(void* addr, uint64_t page_size) ;
int check_page_hash(void* addr, uint64_t page_size) ;
void test_pf_alloc();

/* kernel main function */
int kmain(uint64_t rbx) {
    //int gdb = 1;
    void *mbr_ptr = (void*) (rbx & 0xFFFFFFFF);
    uint64_t *vptr = (uint64_t *) 0xB0BBEEFF000;
    disable_interrupts();
    init_ps2();
    enable_interrupts();
    MMU_init(mbr_ptr);
    printk("hello world\n");
   // while(gdb);
    vpage_alloc((uint64_t) vptr);
    //vpage_alloc(0xAB0BBEEFF000);
    printk("vptr val: %lu\n", * vptr);
    *vptr = 0xFEEDBEEF;
    printk("vptr val: %lx\n", * vptr);
    vpage_free((uint64_t) vptr);
    printk("fault: %lx\n", * vptr);
    while(1);
    return 0;
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
        // every 7, free old page and update old_page to curr_page
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
        if (*(char *)addr != hash) {
            printk("test fail");
            return 0;
        }
    }
    return 1;
}