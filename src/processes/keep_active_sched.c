#include "proc.h"
#include "print.h"

static thread active_head;
static int active_unlinked; // go to next thread if active is unlinked
static int size;

void kaInit();
void kaShutdown();
void kaAdmit(thread admit);
void kaRemove(thread victim);
void kaUnlink(thread victim);
thread kaNext();
thread kaActive();
int kaSize();
thread kaSetActive(thread target);

struct scheduler kaSched = {
    kaInit,
    kaShutdown,
    kaAdmit,
    kaRemove,
    kaUnlink,
    kaNext,
    kaActive,
    kaSize,
    kaSetActive
};
scheduler RunActiveToCompletion = &kaSched;

void kaInit() {
    active_head = 0;
    size = 0;
    active_unlinked = 0;
}

void kaShutdown() {}
void kaAdmit(thread new_thread) {
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

void kaRemove(thread victim) {
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

void kaUnlink(thread victim) {
    thread temp;
    int i;
    if (victim == 0) {
        printk("Unlink Failed: NULL Pointer\n");
        asm volatile("hlt");
    } 
    if (size == 0) {
        return;
    }
    if (size == 1) {
        // unlinked, so failure if rrNext is called
        victim->next = 0;
        victim->prev = 0;
        active_unlinked = 1;
        size--;
        return;
    }
    temp = active_head;
    for (i = 0; i < size; i++) {
        if (temp->tid == victim->tid) {
            temp->next->prev = temp->prev;
            temp->prev->next = temp->next;
            active_unlinked = 1;
            size--;
            return;
        }
        temp = temp->next;
    }
    return;
}

thread kaNext() {
    if (active_unlinked || (active_head != 0 && active_head->stack == 0)) {
        // go to next if active is main to let threads run
        // or if active is unlinked
        active_head = active_head->next;
        active_unlinked = 0;
    }
    return active_head;
}
thread kaActive() {
    return active_head;
}
int kaSize() {
    return size;
}

thread kaSetActive(thread target) {
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