#include "print.h"
#include "vga.h"
#include <stdarg.h>

#define MAXBUF (sizeof(long long int) * 8) // long enough even for binary

void print_int(int i);

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
                print_int(va_arg(args, int));
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

void print_int(int i) {
    char buf[MAXBUF];
    int negative = 0, idx = 0, temp;
    if (i == 0) {
        VGA_display_char('0');
        return;
    }
    if (i < 0) {
        i = -i;
        negative = 1;
    }
    // convert ints to chars and put onto stack
    while (i > 0 && idx < MAXBUF) {
        temp = i % 10;
        i /= 10;
        buf[idx] = temp + 48;
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