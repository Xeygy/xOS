#ifndef PS2_H
#define PS2_H

/* Polls the keyboard for the next character */
int init_ps2();
char ps2_read();
void keyboard_handler();
/* blocks calling thread and returns when kb has a char */
char getc();
#endif