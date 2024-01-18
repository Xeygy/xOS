#include "vga.h"
#include "string.h"
#include "print.h"

#define TO_CHAR(i) (i % 10 + 48)

/* kernel main function */
int kmain() {
    int i, j;
    char num1, num2;
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
    printk("Hello World! len: %d, expected: %d, percent %%\n", strlen(st), 12);
    printk("looong: %d\n", -1234567890);
    printk("Error tests %O %% \n%x % ds %");
    st = "";
    printk("\nempty len: %d\n", strlen(st));
    asm( "hlt" );
}