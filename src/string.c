#include "string.h"

/*
    Sets the block of memory at dst to 
    n copies of value c. c is treated as an
    unsigned char. Returns dst.
*/
void *memset(void *dst, int c, size_t n) {
    char *p;
    int i;
    p = (char *) dst;
    for (i = 0; i < n; i++) {
        p[i] = (unsigned char) c;
    }
    return dst;
}

/*
    Copies n bytes from src to dest. Does not check for 
    overlap or overflow. Returns dest. 
*/
void *memcpy(void *dest, const void *src, size_t n) {
    char *dp, *sp;
    int i;
    dp = (char *) dest;
    sp = (char *) src;
    for (i = 0; i < n; i++) {
        dp[i] = sp[i];
    }
    return dest;
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

/*
    Copies string src into dest, including terminating 0.
    Returns dest. Does not check overflow. 
*/
char *strcpy(char *dest, const char *src) {
    int s_len, i;
    s_len = strlen(src);
    for (i = 0; i <= s_len; i++) {
        dest[i] = src[i];
    }
    return dest;
}
