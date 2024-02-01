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
    int i,j;
    disable_interrupts();
    printk("%lu %lu %lu" , sizeof(uint8_t), sizeof(uint16_t), sizeof(uint32_t));
    init_ps2();
    enable_interrupts();
    while(1);
    return 0;
}