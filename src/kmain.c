#include "print.h"
#include "ps2.h"
#include "interrupts.h"
#include "serial.h"
#include "page_alloc.h"
#include "asm.h"
#include "string.h"
#include "page_alloc.h"
#include "kmalloc.h"
#include "proc.h"
#include <stdint.h>
#include <limits.h>

void test_threads(void* arg);

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
    
    syscall(42);
    PROC_create_kthread(&test_threads, (void *) 0xbeefcafe);
    PROC_run();
    return 0;
}

void test_threads(void* arg) {
    printk("I am thread %p", arg);
}