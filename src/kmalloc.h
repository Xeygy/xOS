#ifndef KMALLOC_H
#define KMALLOC_H

#include <stdint.h>
/* free a malloc'd addr */
void kfree(void *addr);
/* get an allocated block of size bytes*/
void *kmalloc(size_t size);

void *kcalloc(size_t nmemb, size_t size);
void *krealloc(void *ptr, size_t size);
void kMallocPrintHeap();

#endif