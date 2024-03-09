#include "print.h"
#include "ps2.h"
#include "interrupts.h"
#include "page_alloc.h"
#include "proc.h"
#include "snakes.h"
#include "syscall.h"
#include <stdint.h>

void test_threads(void* arg);
void keyboard_io(void *);

/* kernel main function */
int kmain(uint64_t rbx) {
    // int gdb = 1;
    // while(gdb);
    void *mbr_ptr = (void*) (rbx & 0xFFFFFFFF);
    MMU_init(mbr_ptr);
    disable_interrupts();
    init_ps2();
    enable_interrupts();
    init_proc();

    // syscall(SYS_TEST);
    // PROC_create_kthread(&test_threads, (void *) 6);
    // PROC_create_kthread(&test_threads, (void *) 8);
    PROC_create_kthread(&keyboard_io, (void *) 0);
    setup_snakes(0);
    PROC_run();
    while (1) {
        asm volatile ("hlt");
    }
}

void test_threads(void* arg) {
    int i = 0;
    for (i = 1; i <= (uint64_t) arg; i++) {
        printk("%d) I am thread %p\n", i, arg);
        yield();    
    }
}

void keyboard_io(void *) {
   while (1)
      printk("%c", getc());
}