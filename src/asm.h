#ifndef ASM_H
#define ASM_H

/* INLINE ASSEMBLY HELPERS */

#include "stdint.h"
extern inline uint8_t inb(uint16_t port);
extern inline void outb(uint16_t port, uint8_t val);

#endif