#ifndef PAGE_ALLOC_H
#define PAGE_ALLOC_H

/* allocate a page frame */
void * MMU_pf_alloc();
/* free a page frame */
void MMU_pf_free(void *pf);
/* init MMU, takes head of multiboot 2 info structure */
void MMU_init(void *mb2_head);

#endif