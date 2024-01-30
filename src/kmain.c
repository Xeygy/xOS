#include "print.h"
#include "ps2.h"
#include "interrupts.h"
#include <stdint.h>
#include <limits.h>

/* kernel main function */
int kmain() {
    // int gdb_loop = 1;
    //while(gdb_loop);
    printk("%lu %lu %lu" , sizeof(uint8_t), sizeof(uint16_t), sizeof(uint32_t));
    enable_interrupts();
    while (1) {
        printk("%c", getchar());
        printk("%d", 1/0);
    }
    return 0;
}