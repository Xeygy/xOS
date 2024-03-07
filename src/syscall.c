#include "syscall.h"
#include "print.h"

#define MAX_SYSCALL_CT 256

static fp handlers[MAX_SYSCALL_CT];

void syscall_handler(syscall_num_t syscall_num) {
	fp fun;
	fun = handlers[syscall_num];
	if (fun != 0) {
		fun();
	} else {
		printk("syscall: nothing registered for %d\n", syscall_num);
	}
}

void syscall_register_handler(syscall_num_t syscall_num, fp fun) {
	handlers[syscall_num] = fun;
}