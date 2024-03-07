#include "proc.h"
#include "print.h"
#include "kmalloc.h"
#include "asm.h"
#include "syscall.h"
#include <stdint.h>

typedef struct __attribute__ ((aligned(16))) __attribute__ ((packed))
registers {
  unsigned long rax;            /* the sixteen architecturally-visible regs. */
  unsigned long rbx;
  unsigned long rcx;
  unsigned long rdx;
  unsigned long rsi;
  unsigned long rdi;
  unsigned long rbp;
  unsigned long rsp;
  unsigned long r8;
  unsigned long r9;
  unsigned long r10;
  unsigned long r11;
  unsigned long r12;
  unsigned long r13;
  unsigned long r14;
  unsigned long r15;
  //struct fxsave fxsave;   /* space to save floating point state */
} rfile;

typedef struct threadinfo_st *thread;
typedef struct threadinfo_st {
  uint64_t      tid;            /* lightweight process id  */
  unsigned long *stack;         /* Base of allocated stack */
  uint64_t      stacksize;      /* Size of allocated stack */
  rfile         state;          /* saved registers         */
  unsigned int  status;         /* exited? exit status?    */
  thread        next;        
  thread        prev;       
  thread        sched_one;      /* Two more for            */
  thread        sched_two;      /* schedulers to use       */
  thread        exited;         /* and one for lwp_wait()  */
} context;

extern void swap_rfiles(rfile *old, rfile *new); // from magic.asm

static thread make_new_active_thread(void);
static void lwp_wrapper(kproc_t fun, void *arg);
static void yield_sys();

static thread active_head = 0;
static thread proc_run_thread = 0;

/* must be called before using proc */
void init_proc() {
    syscall_register_handler(SYS_YIELD, yield_sys);
}

void yield() {
    syscall(SYS_YIELD);
}

/* to yield*/
static void yield_sys() {
    thread next = 0;
    thread prev = 0;
    /* Invalid Call to Yield, or Nothing to Schedule*/
    if  (active_head == 0)
    {
        printk("Yield Error: Current is Null\n");
        asm volatile("hlt");
    }
    /* No Next, Exit */
    next = active_head->next;
    if (next == 0)
    {
        printk("Yield Error: No Thread to Switch\n");
        asm volatile("hlt");
    }
    /* If the thread is rescheduled, just return immediately */
    if ( next == active_head) {
        return;
    }
    prev = active_head;
    active_head = next;
    
    /* Magic */
    swap_rfiles(&(prev->state), &(next->state));
    return;
}

/* 
Terminate calling lwp, remove from active,
put in exited list and yield 
*/
void exit() {
    asm volatile ("int $0x81");
}

void sys_exit() {
    kfree(active_head->stack);
    // if something in scheduler, remove current from list
    active_head->prev->next = active_head->next;
    active_head->next->prev = active_head->prev;
    active_head = active_head->next;

    swap_rfiles(0, &(active_head->state));
}

/* runs threads until all are exited */
void PROC_run() {
    thread main_thread = make_new_active_thread();
    active_head = main_thread;
    /* Set Stack to null to designate main thread */
    main_thread -> stack = 0;
    proc_run_thread = main_thread;
    // if other things in scheduler, yield()
    while (active_head -> next != active_head) {
        yield();
    }
    printk("done\n");
    return;
}

/* create a thread with function entry_point and add it to active 
threads list */
void PROC_create_kthread(kproc_t entry_point, void *arg) {
    thread new_thread;
    uintptr_t * stack_top;

    new_thread = make_new_active_thread();
    new_thread -> stacksize = 4096; // idk
    new_thread -> stack = kmalloc(new_thread->stacksize);
    new_thread -> state.rdi     = (unsigned long) entry_point;
    new_thread -> state.rsi     = (unsigned long) arg;

    stack_top = (uintptr_t *) (((void *) (new_thread -> stack)) 
            + new_thread -> stacksize - sizeof(uintptr_t));
    /* Caller's Top of Stack: (value unimportant) */
    *(stack_top--) = (uintptr_t) 0;
    /* Next on stack: Return Address */
    *(stack_top--) = (uintptr_t) &lwp_wrapper;
    /* Next: Base Pointer (Can Be Null, No Previous Frame)*/
    new_thread -> state.rbp = (unsigned long) stack_top;
    *(stack_top--) = (uintptr_t) 0;
    /* Set Stack Pointer to Top of Stack */
    new_thread -> state.rsp = (unsigned long) stack_top;
    
    return;
}

/* 
Call the given lwpfunction with the given argument, then
calls lwp_exit() with its return value.
*/
static void lwp_wrapper(kproc_t fun, void *arg) {
    fun(arg); 
    exit();
    return;
}

/*
mallocs a new thread and adds it to the active threads list
*/
static thread make_new_active_thread() {
    thread new_thread;
    new_thread = kmalloc(sizeof(context));
    new_thread -> tid = (uint64_t) new_thread;
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

    /* Add Thread to Scheduler */
    //sched_admit(new_thread);
    return new_thread;
}