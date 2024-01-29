#include "print.h"
#include "ps2.h"
#include "interrupts.h"
#include <limits.h>

/* kernel main function */
int kmain() {
    // int gdb_loop = 1;
    //while(gdb_loop);
    enable_interrupts();
    while (1) {
        printk("%c", getchar());
    }
    return 0;
}