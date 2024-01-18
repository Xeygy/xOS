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
    printk("Hello World! len: %u, expected: %d, percent %%\n", strlen(st), 12);
    printk("int min: %d, max: %d\n", INT_MIN, INT_MAX);
    printk("uint max: %u, overflow (+1): %u \n", UINT_MAX, UINT_MAX + 1);
    printk("Error tests %O %% \n%x % ds %");
    st = "";
    printk("\nempty len: %u\n", strlen(st));
    return 0;
}