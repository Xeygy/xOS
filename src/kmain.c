#include "print.h"
#include "ps2.h"
#include "interrupts.h"
#include "serial.h"
#include "page_alloc.h"
#include "asm.h"
#include <stdint.h>
#include <limits.h>

/* kernel main function */
int kmain(uint64_t rbx) {
    // int gdb_loop = 1;
    // while(gdb_loop);
    void *mbr_ptr = (void*) (rbx & 0xFFFFFFFF);
    disable_interrupts();
    init_ps2();
    enable_interrupts();
    MMU_init(mbr_ptr);
    while(1);
    return 0;
}