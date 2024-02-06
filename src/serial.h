#ifndef SERIAL_H
#define SERIAL_H

/* 
writes to serial port
returns 0 on success, error on fail
*/
int SER_write(const char *buff, int len);

#endif