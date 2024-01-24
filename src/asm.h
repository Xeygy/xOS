#ifndef ASM_H
#define ASM_H

#include "stdint.h"

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
#endif