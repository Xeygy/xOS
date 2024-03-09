#include "proc.h"
#include "print.h"

static thread active_head;
static int size;

void rrInit();
void rrShutdown();
void rrAdmit(thread admit);
void rrRemove(thread victim);
thread rrNext();
thread rrActive();
int rrSize();
thread rrSetActive(thread target);

struct scheduler rrSched = {
    rrInit,
    rrShutdown,
    rrAdmit,
    rrRemove,
    rrNext,
    rrActive,
    rrSize,
    rrSetActive
};
scheduler RoundRobin = &rrSched;

void rrInit() {
    active_head = 0;
    size = 0;
}
void rrShutdown() {}
void rrAdmit(thread new_thread) {
    /* Maintain Doubly Linked List */
    if ( active_head == 0 ) {
        active_head = new_thread;
        new_thread -> next = new_thread;
        new_thread -> prev = new_thread;
    } else {
        // insert right before active_head
        new_thread -> prev = active_head -> prev;
        new_thread -> next = active_head;
        active_head -> prev -> next = new_thread;
        active_head -> prev = new_thread;
    }
    size++;
}

void rrRemove(thread victim) {
    thread temp = 0;
    int i;
    if (victim == 0) {
        printk("Remove Failed: NULL Pointer\n");
        asm volatile("hlt");
    } 
    if (size == 0) {
        return;
    }
    if (size == 1) {
        size--;
        active_head = 0;
        return;
    }
    temp = active_head;
    for (i = 0; i < size; i++) {
        if (temp->tid == victim->tid) {
            if (temp == active_head) {
                active_head = temp->next;
            }
            temp->next->prev = temp->prev;
            temp->prev->next = temp->next;
            temp->prev = 0;
            temp->next = 0;
            size--;
            return;
        }
        temp = temp->next;
    }
    return;
}
thread rrNext() {
    if (active_head == 0 || active_head->next == 0) {
        printk("Sched Error: No Thread to Switch\n");
        asm volatile("hlt");
    }
    active_head = active_head->next;
    return active_head;
}
thread rrActive() {
    return active_head;
}
int rrSize() {
    return size;
}

thread rrSetActive(thread target) {
    int i;
    thread temp;
    temp = active_head;
    for (i = 0; i < size; i++) {
        if (target->tid == temp->tid) {
            active_head = temp;
            return active_head;
        }
        temp = temp->next;
    }
    return active_head;
}