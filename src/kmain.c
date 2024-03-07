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
#include "syscall.h"
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
    init_proc();

    syscall(SYS_TEST);
    PROC_create_kthread(&test_threads, (void *) 0xbeefcafe);
    PROC_create_kthread(&test_threads, (void *) 0x13371337);
    PROC_run();
    while (1){
        asm volatile ("hlt");
    }
}

void test_threads(void* arg) {
    int i = 0;
    for (i = 0; i < 10; i++) {
        printk("%d) I am thread %p\n", i, arg);
        yield();    
    }
}