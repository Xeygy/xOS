#include "vga.h"
#include "string.h"
#include "print.h"
#include "ps2.h"
#include <limits.h>

#define TO_CHAR(i) (i % 10 + 48)

/* kernel main function */
int kmain() {
    int gdb_loop = 1;
    //while(gdb_loop);
    while (1) {
        printk("%c", getchar());
    }
    return 0;
}