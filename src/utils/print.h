#ifndef PRINT_H
#define PRINT_H

int printk(const char *fmt, ...) __attribute__ ((format (printf, 1, 2)));

typedef enum {
    DPRINT_QUIET = 0,   // The most minimal output
    DPRINT_NORMAL = 1,  // Standard amount of debug text
    DPRINT_DETAILED = 2 // Verbose output
} dprint_verbosity_t;
// set verbosity of debug print statements
void set_debug_verbosity(dprint_verbosity_t verbosity);
int dprintk(dprint_verbosity_t verbosity, const char *fmt, ...);

#endif