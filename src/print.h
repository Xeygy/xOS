#ifndef PRINT_H
#define PRINT_H

int printk(const char *fmt, ...) __attribute__ ((format (printf, 1, 2)));

#endif