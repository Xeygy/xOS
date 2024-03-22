#ifndef FAT32_INTERFACE_H
#define FAT32_INTERFACE_H

#include "fat32.h"
/***********************************/
/* some functions for easy         */
/* interaction with the filesystem */
/* (slightly more abstracted)      */
/***********************************/

/* 
opens the file at pathname for reading
and returns it, null if it doesn't exist 
*/
File ropen(const char *pathname);

/* print out first num_bytes of the file at pathname */
void f32_print_contents(const char *pathname, int num_bytes);

/* prints the contents of the directory at pathname */
// void f32_ls(const char *pathname);

/* prints the md5 hash of the file at pathname */
void f32_md5(const char *pathname);

#endif