#ifndef ASM_H
#define ASM_H

#include <stdint.h>

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
static inline void ltr(uint64_t offset) {
    asm volatile ("ltr %0" 
                :
                : "m"(offset)); // TODO: Is this correct?
}

/*
    enables interrupts (set interrupt flag)
*/
static inline void sti(void) {
    asm volatile ("sti");
}

/*
    disables interrupts (clear interrupt flag)
*/
static inline void cli(void) {
    asm volatile ("cli");
}
#endif