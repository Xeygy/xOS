#include "vga.h"
#include "string.h"

#define TO_CHAR(i) (i % 10 + 48)

/* kernel main function */
int kmain() {
    int i, j;
    char num1, num2;
    char *st;
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 10; j++) {
            num1 = TO_CHAR(i);
            num2 = TO_CHAR(j);
            VGA_display_str("Hello World ");
            VGA_display_char(num1);
            VGA_display_char(num2);
            VGA_display_str("\n");
        }
    }
    st = "Hello";
    VGA_display_str("Hello len: ");
    VGA_display_char(TO_CHAR(strlen(st)));
    st = "";
    VGA_display_str("\nempty len: ");
    VGA_display_char(TO_CHAR(strlen(st)));
    asm( "hlt" );
}