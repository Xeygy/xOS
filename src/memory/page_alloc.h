#ifndef PAGE_ALLOC_H
#define PAGE_ALLOC_H

#include <stdint.h>
/* allocate a page frame */
void * MMU_pf_alloc();
/* free a page frame */
void MMU_pf_free(void *pf);
/* init MMU, takes head of multiboot 2 info structure */
int MMU_init(void *mb2_head);

/* page fault handler */
void pf_isr(uint64_t error_code);

/* allocates the page at virt_addr */
void *vpage_alloc(uint64_t virt_addr);
/* frees the page at virt_addr */
void *vpage_free(uint64_t virt_addr);

void *sbrk(signed long incr);

#endif