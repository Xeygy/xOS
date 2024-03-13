#ifndef INTERRUPTS_H
#define INTERRUPTS_H

/* returns 0 if successful, error code otherwise */
int interrupts_enabled();
void firstTimeSetup();
void p_int();
#endif