#ifndef FS_UTILS_H
#define FS_UTILS_H

#include <stdint.h>

typedef struct StringBuilder {
    char *segment;
    int idx;
    uint64_t tot_char_ct, segment_len;
    struct StringBuilder *next;
} __attribute__((packed)) StringBuilder;

typedef struct FilePath {
    char *name;
    struct FilePath *next;
} FilePath;

/* converts the string into malloc'd file path pointer list */
FilePath *split_fpath(char *str, char delim);

/* inserts new_val into pile such that prev->idx <= idx <= next->idx*/
StringBuilder *insert_sb(StringBuilder *pile, char *new_val, int idx);

/* return the malloc'd string represented by sb and free sb */
char *build_string(StringBuilder *sb);

#endif