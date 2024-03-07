#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

typedef enum {
	SYS_YIELD = 1,
	SYS_EXIT = 2,
    SYS_TEST = 42
} syscall_num_t;

typedef void (*fp)();

void syscall_handler(syscall_num_t syscall_num);
void syscall_register_handler(syscall_num_t syscall_num, fp fun);

#endif