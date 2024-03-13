#include "print.h"
#include "string.h"
#include "kmalloc.h"
#include "page_alloc.h"

#define CHUNK_SIZE 2048 /* must be > sizeof(header)*/

/******************************************/
/* This code is from a simple linked-list */
/* malloc I wrote in CPE 453.             */
/******************************************/

typedef struct header {
    struct header *next, *prev; /* points to next, prev header */
    size_t size; /* size of the block in B, including header */
    int inUse; /* bool if in use or not */
} Header;

static Header *heap;
static int heapInit = 0;
static int debugMode = 0;

int initializeHeap();
void shrinkH(Header *h, size_t needed);

void *mallocRaw(size_t size);
void *callocRaw(size_t nmemb, size_t size);
void *reallocRaw(void *ptr, size_t size);
void freeRaw(void *ptr);

/* 
set to 0 to disable babbling, 
otherwise babble away 
*/
void kmalloc_set_babble(int val) {
    debugMode = val;
}
/* wrapper around mallocRaw for print debugging */
void *kmalloc(size_t size) {
    void *res = mallocRaw(size);
    void *start = NULL;
    Header *h = NULL;
    if (debugMode) {
        if (res == NULL) {
            printk("MALLOC: malloc(%d) => (ptr=%p,size=%d) \n",
            (int) size, NULL, 0);
        } else {
            start = (void *) ((uintptr_t) res - sizeof(Header));
            h = (Header *) start;
            printk("MALLOC: malloc(%d) => (ptr=%p,size=%d) \n",
            (int) size, start, (int) h->size);
        }
    }
    return res;
}

/* wrapper around callocRaw for print debugging */
void *kcalloc(size_t nmemb, size_t size) {
    void *res = callocRaw(nmemb, size);
    void *start = NULL;
    Header *h = NULL;
    if (debugMode) {
        if (res == NULL) {
            printk("MALLOC: calloc(%d,%d) => (ptr=%p, size=%d) \n",
            (int) nmemb, (int) size, NULL, 0);
        } else {
            start = (void *) ((uintptr_t) res - sizeof(Header));
            h = (Header *) start;
            printk("MALLOC: calloc(%d,%d) => (ptr=%p, size=%d) \n",
            (int) nmemb, (int) size, start, (int) h->size);
        }
    }
    return res;
}

/* wrapper around reallocRaw for print debugging */
void *krealloc(void *ptr, size_t size) {
    void *res = reallocRaw(ptr, size);
    void *start = NULL;
    Header *h = NULL;
    if (debugMode) { 
        if (res == NULL) {
            printk("MALLOC: realloc(%p,%d) => (ptr=%p,size=%d) \n",
            ptr, (int) size, NULL, 0);
        } else {
            start = (void *) ((uintptr_t) res - sizeof(Header));
            h = (Header *) start;
            printk("MALLOC: realloc(%p,%d) => (ptr=%p,size=%d) \n",
            ptr, (int) size, start, (int) h->size);
        }
    }
    return res;
}

/* wrapper around freeRaw for print debugging*/
void kfree(void *ptr) {
    freeRaw(ptr);
    if (debugMode) {
        if (ptr==NULL) {
            printk("MALLOC: free(NULL) \n");       
        } else {
            printk("MALLOC: free(%p) \n", ptr);
        }
    }
}

/*
allocates  size  bytes  and returns a pointer to the allocated memory.  
The memory is not initialized.  If size is 0, then malloc() returns
either NULL, or a unique pointer value that can later be 
successfully passed to free().
*/
void *mallocRaw(size_t size) {
    Header *p, *prev = NULL;
    size_t units, needed;

    if (prev) {
        return NULL;
    }

    if (size == 0) {
        return NULL;
    }
    if (!heapInit) {
        if(!initializeHeap()){
            return NULL;
        }
    }
    
    units = (size/16 + 1);  /* round up */
    for (p = heap; ; p = p->next) {
        needed = 16 * units + sizeof(Header);
        if (!p->inUse 
            && p->size >= needed 
            && p->size < needed + sizeof(Header)) {
            /* the space requested will take up entire p->size */
            p->inUse = 1;
            return (char *)p + sizeof(Header);
        } else if (!p->inUse && p->size > needed) {
            /* the space requested will not take up entire size */
            p->inUse = 1;
            /* init p->next */
            shrinkH(p, needed);
            return (char *)p + sizeof(Header);
        }
        
        /* loop at the end, allocate another chunk */
        if (p->next == heap) {
            if (needed < CHUNK_SIZE) {
                needed = CHUNK_SIZE;
            } 
            /* sbrk + error handling*/
            if (sbrk(needed) == (void *) -1) {
                return NULL;
            }
            /* init new chunk */
            p->next = (Header *) ((char *) p + p->size);
            p->next->next = heap;
            p->next->prev = p;
            p->next->inUse = 0;
            p->next->size = needed;
            heap->prev = p->next;
        }
        prev = p;
    }
}

/* 
returns 0 if heap init goes wrong,
returns 1 on success 
align on 16B
use char as a standin for 1B 
*/
int initializeHeap() {
    uintptr_t offset, sb;
    void *t = sbrk(0);
    if ((sb = (uintptr_t) t) < 0) {
        return 0;
    }
    /* note that offset is always > 0 */
    offset = 16 - sb % 16; 

    if ((sb = (uintptr_t) sbrk(CHUNK_SIZE + offset)) < 0) {
        return 0;
    }
    heap = (Header *) (sb + offset);
    heap->next = heap->prev = heap;
    heap->size = CHUNK_SIZE;
    heap->inUse = 0;
    heapInit = 1;
    kMallocPrintHeap();
    return 1;
}

/*
if ptr is not memory that is malloc'd undefined behavior occurs (i.e. 
ok to assume ptr is malloc'd -> initializeHeap() has been called) 
If ptr is NULL, no operation is performed.
Note invariant that after each call to free, there are no adjacent
headers who are both not inUse (i.e. empty headers are merged)
*/
void freeRaw(void *ptr) {
    Header *curr;
    if (ptr == NULL || !heapInit) {
        return;
    }
    for (curr = heap; ; curr = curr->next) {
        /* if ptr is within the bounds of current chunk, free memory */
        if ((char *)curr <= (char *)ptr 
            && (char *)ptr < (char *)curr + curr->size) {
            curr->inUse = 0;
            if (curr->next != heap && !curr->next->inUse) {
                /* merge next with curr */
                curr->size = curr->next->size + curr->size;
                curr->next = curr->next->next;
                curr->next->prev = curr;
            }
            if (curr != heap && !curr->prev->inUse) {
                /* merge curr with prev */
                curr->prev->next = curr->next;
                curr->prev->size = curr->prev->size + curr->size;
                curr->next->prev = curr->prev;
            }
            return;
        }
    }
}

/*
allocates memory for an array of nmemb elements of 
size bytes each and returns a pointer to the allocated memory.
The memory is set to zero.  If nmemb or size is  0,  
then calloc() returns NULL.
*/
void *callocRaw(size_t nmemb, size_t size) {
    size_t totalBytes;
    void *ans = NULL;
    if (nmemb == 0 || size == 0) {
        return NULL;
    }
    totalBytes = nmemb*size;
    /* overflow, refuse call */
    if (totalBytes/size != nmemb) {
        return NULL;
    }

    if((ans = mallocRaw(totalBytes)) == NULL){
        return NULL;
    }
    memset(ans, 0, totalBytes); /*set memory to 0*/
    return ans;
}

/* realloc returns to start of block */        
void *reallocRaw(void *ptr, size_t size) {
    Header *curr, *temp;
    size_t units, needed;
    if (ptr == NULL) {
        return mallocRaw(size);
    } else if (size == 0) {
        freeRaw(ptr);
        return NULL;
    }
    units = (size/16 + 1);  /* round up */
    needed = 16 * units + sizeof(Header);
    for (curr = heap; ; curr = curr->next) {
        /* if ptr is within the bounds of current chunk, realloc */
        if ((char *)curr <= (char *)ptr 
            && (char *)ptr < (char *)curr + curr->size){
            if (curr->size > needed) {
                /* shrinking by less than a header, do nothing */
                if (needed + sizeof(Header) > curr->size) {
                    return (char *)curr + sizeof(Header);
                }
                /* shrink, init curr->next*/
                shrinkH(curr, needed);
                freeRaw(curr->next); /* merge */
                return (char *)curr + sizeof(Header);
            } else if (curr->size < needed) {
                /* in place expansion (only look above) */
                if (curr->next != heap 
                    && !curr->next->inUse 
                    && curr->next->size + curr->size > needed) {
                    curr->size = curr->next->size + curr->size;
                    curr->next = curr->next->next;
                    curr->next->prev = curr;
                    if (curr->size >= needed + sizeof(Header)) {
                        shrinkH(curr, needed);
                        freeRaw(curr->next); /* merge */
                    }
                    return (char *)curr + sizeof(Header);
                }
                /* use malloc to expand elsewhere */
                if ((temp = mallocRaw(size)) == NULL) {
                    return NULL;
                } else {
                    memcpy(temp,
                        (void *)((uintptr_t)curr + sizeof(Header)),
                        curr->size - sizeof(Header));
                    freeRaw(curr);
                    return temp;
                }
            } else {
                /* mem size requested = actual mem size */
                return (char *)curr + sizeof(Header);
            }
        }
    }
}

/* 
needed < h->size + sizeof(Header) 
shrinks a given header h to a smaller size (needed)
and opens up the space
*/
void shrinkH(Header *h, size_t needed) {
    Header *temp = h->next;
    h->next = (Header *) ((char *) h + needed);
    h->next->next = temp;
    h->next->prev = h;
    temp->prev = h->next;
    h->next->inUse = 0;
    h->next->size = h->size - needed;
    h->size = needed;
}

void kMallocPrintHeap() {
    Header *p = heap;
    dprintk(DPRINT_DETAILED, "-------- malloc heap\n");
    do {
        dprintk(DPRINT_DETAILED, "pointer %p, (%p, %p), inUse %d, size %d \n", 
        (void *)p, (void *)p->prev,  
        (void *)p->next, p->inUse, (int)p->size);
        p = p->next;
    } while (p != heap);
    dprintk(DPRINT_DETAILED, "-------- \n");
}