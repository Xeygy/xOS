#include "print.h"
#include "vga.h"
#include <stdarg.h>

#define MAXBUF (sizeof(long long int) * 8) // long enough even for binary
static char digits[] = "0123456789abcdef";

void print_llong(long long int i, int base);

/*
    printf but for the kernel. supports
        %% - a percent sign
        %d - a signed int
        %u - an unsigned int
        %x - lowercase hex representation
        %c - unsigned char
        %p - pointer in hex prefixed with 0x
        %s - string (char ptr w/terminator)

        %h[dux] - converts to a short 
        %l[dux] - converts to a long
        %q[dux] - converts to a long long
*/
int printk(const char *fmt, ...) {
    int i = 0, num_vars = 0, fstr;
    va_list args;
    va_start(args, fmt); 

    while (fmt[i] != '\0') {
        if (fmt[i] != '%') {
            VGA_display_char(fmt[i]);
            i++;
            continue;
        }
        i++;
        switch (fmt[i]) {
            case '%':
                VGA_display_char('%');
                break;
            case 'd':
                print_llong(va_arg(args, int), 10);
                break;
            case '\0':
                VGA_display_str(" <trailing %>");
                i--; //to catch the null terminator
                break;
            default:
                VGA_display_str("<undefined specifier: ");
                VGA_display_char(fmt[i]);
                VGA_display_char('>');
            }
        i++;
    }
    va_end(args);
}

/* base must be between 1 and 16 inclusive */
void print_llong(long long int i, int base) {
    char buf[MAXBUF];
    int negative = 0, idx = 0, temp;
    if (i == 0) {
        VGA_display_char('0');
        return;
    }
    if (i < 0) {
        negative = 1;
    }
    // convert ints to chars and put onto stack
    while (i != 0 && idx < MAXBUF) {
        temp = i % base;
        if (negative)
            temp = -temp;
        i /= base;
        buf[idx] = digits[temp];
        idx++;
    }
    idx--; // revert the last ++
    if (negative)
        VGA_display_char('-');
    // pop chars off the stack and display
    while (idx >= 0) {
        VGA_display_char(buf[idx]);
        idx--;
    }
}