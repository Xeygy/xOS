#include "print.h"
#include "vga.h"
#include <stdarg.h>

#define MAXBUF (sizeof(long long int) * 8) // long enough even for binary
static char digits[] = "0123456789abcdef";

void print_ullong(unsigned long long int i, int base);
void print_llong(long long int i, int base);

/*
    printf but for the kernel. supports
        %% - a percent sign
        %d - a signed int
        %u - an unsigned int
        %x - lowercase hex uint
        %c - unsigned char
        %p - pointer in hex prefixed with 0x
        %s - string (char ptr w/terminator)

        %h[dux] - converts to a short 
        %l[dux] - converts to a long
        %q[dux] - converts to a long long
*/
int printk(const char *fmt, ...) {
    int i = 0;
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

            case 'u':
                print_ullong(va_arg(args, unsigned int), 10);
                break;

            case 'p':
                VGA_display_str("0x");
            case 'x':
                print_ullong(va_arg(args, unsigned int), 16);
                break;

            case 'c':
                VGA_display_char(va_arg(args, int));
                break;

            case 's':
                VGA_display_str(va_arg(args, char *));
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
    return 0;
}

void print_llong(long long int i, int base) {
    unsigned long long u;
    if (i < 0) {
        u = -i;
        VGA_display_char('-');
    } else 
        u = i;
    print_ullong(u, base);
}

/* base must be between 1 and 16 inclusive */
void print_ullong(unsigned long long int i, int base) {
    char buf[MAXBUF];
    int idx = 0, temp;
    if (i == 0) {
        VGA_display_char('0');
        return;
    }
    // convert ints to chars and put onto stack
    while (i != 0 && idx < MAXBUF) {
        temp = i % base;
        i /= base;
        buf[idx] = digits[temp];
        idx++;
    }
    idx--; // revert the last ++
    // pop chars off the stack and display
    while (idx >= 0) {
        VGA_display_char(buf[idx]);
        idx--;
    }
}