#include "asm.h"

/*
    recieves 8 bits from a i/o port
*/
extern inline uint8_t inb(uint16_t port) {
      uint8_t ret;
      asm volatile ( "inb %1, %0"
                    : "=a"(ret)
                    : "Nd"(port) );
return ret; 
}

/*
    sends 8 bits to an i/o port
*/
extern inline void outb(uint16_t port, uint8_t val) {
      asm volatile ( "outb %0, %1" 
                    : 
                    : "a"(val), "Nd"(port) );
}