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
    cli(); // disable interrupts
    printk("%lu %lu %lu" , sizeof(uint8_t), sizeof(uint16_t), sizeof(uint32_t));
    enable_interrupts();
    while (1) {
        printk("%c", getchar());
        asm volatile ("int $0x40");
    }
    return 0;
}