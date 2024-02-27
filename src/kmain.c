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
void testmalloc1();
int *getOne();
void testCalloc();
void testRealloc();
void testRealloc2();
void testRealloc3();
char *getHW();

/* kernel main function */
int kmain(uint64_t rbx) {
    //int gdb = 1;
    void *mbr_ptr = (void*) (rbx & 0xFFFFFFFF);
    //uint64_t *vptr = (uint64_t *) 0xB0BBEEFF000;
    disable_interrupts();
    init_ps2();
    enable_interrupts();
    MMU_init(mbr_ptr);

    // testmalloc1();
    //testCalloc();
    // testRealloc();
    testRealloc2();

    while(1);
    return 0;
}

void testRealloc2() {
    int i;
    void *ptr;
    for (i = 1; i <= 8192; i++) {
        if (i % 2 == 1) {
            ptr =kmalloc(i);
        } else {
            ptr =krealloc(ptr, i);
        }
    }
}

void testCalloc() {
    char *helloWorld;
    char *arr[100];
    int i;
    sbrk(1);
    for (i = 0; i < 100; i++) {
        helloWorld = getHW();
        arr[i] = helloWorld;
    }
    kMallocPrintHeap();

    for (i = 99; i >= 51; i--) {
            kfree(arr[i]);
    }
    for (i = 50; i >= 0; i--) {
            kfree(arr[i]);
    }
    kMallocPrintHeap();
    helloWorld = getHW(); 
    printk("this is one: %s \n", helloWorld);
    arr[i] = helloWorld;
    kfree(helloWorld);
}

void testRealloc() {
    char *st = getHW();
    char *st2, *st3, *st4, *st5;
    int i;
    int len = strlen(LONGTEXT);
    kMallocPrintHeap();
    /* in place expansion */
    st2 =krealloc(st, sizeof(char)*(len+1));
    for (i = 0; i < len; i++) {
        st2[i] = LONGTEXT[i];
    }
    kMallocPrintHeap();
    /* shrink */
    len = strlen(NEATO);
    st3 =krealloc(st2, sizeof(char)*(len+1));
    for (i = 0; i < len; i++) {
        st3[i] = NEATO[i];
    }
    /* find new spot */
    kMallocPrintHeap();
    len = strlen(LONGERTEXT);
    st4 =krealloc(st3, sizeof(char)*(len+1));
    for (i = 0; i < len; i++) {
        st4[i] = LONGERTEXT[i];
    }
    kMallocPrintHeap();
    st5 =krealloc(st4, 999999999);
    if (st5 == NULL) {
        printk("null");
    }
    kMallocPrintHeap();
}


void testmalloc1() {
    int *one, *one2, *one3;
    one = getOne();
    one2 = getOne();
    one3 = getOne();
    printk("this is one: %d \n", *one);
    printk("this is one too: %d \n", *one2);
    kfree(one3);
    one3 = getOne();
    printk("this is one as well: %d \n", *one3);
    one3 = getOne();
    kfree(one3);
    kfree(one);
    kfree(one2);
}
int *getOne() {
    int *one = kmalloc(sizeof(int));
    *one = 1;
    return one;
}

char *getHW() {
    char *hw = NULL;
    int i;
    int len = strlen(HW);
    hw = (char *)kcalloc(sizeof(char), len + 1);
    for (i = 0; i < len; i++) {
        hw[i] = HW[i];
    }
    return hw;
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
        if (*(char *)addr != hash) {
            printk("test fail");
            return 0;
        }
    }
    return 1;
}