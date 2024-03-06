#ifndef KMALLOC_H
#define KMALLOC_H

#include <stddef.h>
#include <stdint.h>
/* free a malloc'd addr */
void kfree(void *addr);
/* get an allocated block of size bytes*/
void *kmalloc(size_t size);
/* 
set to 0 to disable babbling, 
otherwise babble away 
*/
void kmalloc_set_babble(int val);

void *kcalloc(size_t nmemb, size_t size);
void *krealloc(void *ptr, size_t size);
void kMallocPrintHeap();

#endif