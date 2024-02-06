#include "print.h"
#include "ps2.h"
#include "interrupts.h"
#include "serial.h"
#include "asm.h"
#include <stdint.h>
#include <limits.h>

/* kernel main function */
int kmain() {
    // int gdb_loop = 1;
    // while(gdb_loop);
    disable_interrupts();
    init_ps2();
    enable_interrupts();
    SER_write(0, 2);
    while(1);
    return 0;
}