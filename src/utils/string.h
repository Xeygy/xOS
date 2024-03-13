#ifndef STRING_H
#define STRING_H

#include <stddef.h>
/*
    Sets the block of memory at dst to 
    n copies of value c. c is treated as an
    unsigned char. Returns dst.
*/
void *memset(void *dst, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
size_t strlen(const char *s);
char *strcpy(char *dest, const char *src);
// int strcmp(const char *s1, const char *s2);
// const char *strchr(const char *s, int c);
// char *strdup(const char *s);

#endif