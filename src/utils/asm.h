#ifndef ASM_H
#define ASM_H

#include <stdint.h>
/******************************/
/* cleaner inline asm utility */
/******************************/

/*
    sends 8 bits to an i/o port
*/
static inline void outb(uint16_t port, uint8_t val) {
      asm volatile ( "outb %0, %1" 
                    : 
                    : "a"(val), "Nd"(port) );
}

/*
    recieves 8 bits from a i/o port
*/
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %1, %0"
                : "=a"(ret)
                : "Nd"(port) );
    return ret; 
}

/*
    recieves 16 bits from a i/o port
*/
static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    asm volatile ( "inw %1, %0"
                : "=a"(ret)
                : "Nd"(port) );
    return ret; 
}

//idt register
typedef struct {
	uint16_t	limit;
	uint64_t	base;
} __attribute__((packed)) idtr_t;
/*
    loads the address pointed to by idt into the 
    interrupt descriptor table register
    https://stackoverflow.com/a/43577960
*/
static inline void lidt(idtr_t* idtr) {
    asm volatile ("lidt %0" 
                :
                : "m"(*idtr));
}

/*
    sets the task state segment 
*/
static inline void ltr(void *offset) {
    asm volatile ("ltr %0" 
                :
                : "m"(offset)); // TODO: Is this correct?
}

/*
invalidates entry in tlb
*/
static inline void invlpg(void *vaddr) {
    asm volatile ( "invlpg (%0)" : : "b"(vaddr) : "memory" );
}

/*
    calls int 0x80 with syscall num
    returns a value from the syscall
*/
static inline uint64_t syscall(uint64_t num) {
    asm volatile (
        "mov %0, %%rdi\n\t"
        "int $0x80" : : "r" (num)
        );
    // placeholder, just return syscall num for now
    return num;
}

/*
    DO NOT USE FOR GENERAL INTERRUPT ENABLE 
    use the wrapper enable_interrupts() in
    interrupts.h, so we can track if interrupts
    are enabled in software.

    enables interrupts (set interrupt flag)
*/
static inline void sti(void) {
    asm volatile ("sti");
}

/*
    DO NOT USE FOR GENERAL INTERRUPT DISABLE 
    use the wrapper disable_interrupts() in
    interrupts.h, so we can track if interrupts
    are enabled in software.

    disables interrupts (clear interrupt flag)
*/
static inline void cli(void) {
    asm volatile ("cli");
}
#endif