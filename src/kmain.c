#include "vga.h"
#include "string.h"
#include "print.h"
#include <limits.h>

#define TO_CHAR(i) (i % 10 + 48)

/* kernel main function */
int kmain() {
    // int i, j;
    // char num1, num2;
    char *st;
    int gdb_loop = 1;
    /*for (i = 0; i < 5; i++) {
        for (j = 0; j < 10; j++) {
            num1 = TO_CHAR(i);
            num2 = TO_CHAR(j);
            VGA_display_str("Hello World ");
            VGA_display_char(num1);
            VGA_display_char(num2);
            VGA_display_str("\n");
        }
    }*/
    st = "Hello World!";
    printk("Hello World! len: %lu, expected: %d, percent %%\n", strlen(st), 12);
    printk("int min: %d, max: %d\n", INT_MIN, INT_MAX);
    printk("ld: %ld, qd min: %qd, int breaks: %d\n", LONG_MAX, LLONG_MIN, LONG_MAX);
    printk("qu: %qu, lu: %lu\n", ULLONG_MAX, ULONG_MAX);
    printk("qx: %qx, lx: %lx \n", ULLONG_MAX, ULONG_MAX);
    printk("short min: %hd, max: %hd\n", SHRT_MIN, SHRT_MAX);
    printk("uint max: %u, overflow (+1): %u \n", UINT_MAX, UINT_MAX + 1);
    printk("uint hex of 28 is: %x, max: %x \n", 28, UINT_MAX);
    printk("unsigned char test: %c, emoji: %c \n", '&', '🥝');
    printk("pointer test: %p, string test: %s \n", &st, st);
    printk("Error tests %O %% %x % ds %");
    st = "";
    printk("\nempty len: %lu\n", strlen(st));
    return 0;
}