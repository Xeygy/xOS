#ifndef INTERRUPTS_H
#define INTERRUPTS_H

/* returns 0 if successful, error code otherwise */
int enable_interrupts();
int disable_interrupts();

int interrupts_enabled();

#endif