#include <stddef.h>
#include "string.h"

void *memset(void *dst, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
size_t strlen(const char *s);
char *strcpy(char *dest, const char *src);
int strcmp(const char *s1, const char *s2);
const char *strchr(const char *s, int c);
char *strdup(const char *s);

/*
    Sets the block of memory at dst to 
    n copies of value c. c is treated as an
    unsigned char.
*/
void *memset(void *dst, int c, size_t n) {
    char *p;
    int i;
    p = (char *) dst;
    for (i = 0; i < n; i++) {
        p[i] = (unsigned char) c;
    }
}

/*
    Copies n bytes from src to dest. Does not check for 
    overlap or overflow.    
*/
void *memcpy(void *dest, const void *src, size_t n) {
    char *dp, *sp;
    int i;
    dp = (char *) dest;
    sp = (char *) src;
    for (i = 0; i < n; i++) {
        dp[i] = sp[i];
    }
}

/*
    Returns the length of string s.
*/
size_t strlen(const char *s) {
    size_t len = 0;
    while (*s != '\0') {
        s++;
        len++;
    }
    return len;
}