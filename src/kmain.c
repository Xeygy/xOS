#include "print.h"
#include "ps2.h"
#include "interrupts.h"
#include "asm.h"
#include <stdint.h>
#include <limits.h>

/* kernel main function */
int kmain() {
    // int gdb_loop = 1;
    //while(gdb_loop);
    int i;
    disable_interrupts();
    printk("%lu %lu %lu" , sizeof(uint8_t), sizeof(uint16_t), sizeof(uint32_t));
    init_ps2();
    enable_interrupts();
    for (i = 0;; i = ++i%1000) {
        printk("ticking%d...\n", i);
    }
    return 0;
}