#include "proc.h"
#include "print.h"
#include "kmalloc.h"
#include "asm.h"
#include "syscall.h"
#include "schedulers.h"
#include "interrupts.h"


extern void swap_rfiles(rfile *old, rfile *new); // from magic.asm

static thread make_new_active_thread(void);
static void lwp_wrapper(kproc_t fun, void *arg);
static void yield_sys();

static scheduler sched = 0;

/* must be called before using proc */
void init_proc() {
    syscall_register_handler(SYS_YIELD, yield_sys);
    register_sched(RoundRobin);
}

static void push_q(thread_q *q, thread t) {
    thread_node *tn = kmalloc(sizeof(thread_node));
    if (q == 0) {
        printk("can't add to null q");
        return;
    }
    tn->t = t;
    tn->next = 0;
    tn->prev = 0;
    if (q->head == 0) {
        q->head = tn;
        q->tail = tn;
    } else {
        tn->next = q->head;
        q->head->prev = tn;
        q->head = tn;
    }
}

static thread pop_q(thread_q *q) {
    thread ans = 0;
    thread_node *ans_tn=0;
    if (q == 0) {
        printk("can't remove from null q");
        return 0;
    }
    if (q->tail == 0) {
        printk("can't remove from empty q");
        return 0;
    }
    if (q->tail == q->head) {
        q->head = 0;
    }
    ans = q->tail->t;
    ans_tn = q->tail;
    q->tail = q->tail->prev;
    kfree(ans_tn);
    return ans;
}

/* move thread from head of q to scheduler
returns 0 on success, error code on failure */
int PROC_unblock_head(thread_q *q)  {
    if (q == 0 || q->head == 0) {
        return 1;
    }
    sched->admit(pop_q(q));
    return 0;
}

/* move all threads in q to scheduler*/
void PROC_unblock_all(thread_q *q) {
    while (q->head != 0) {
        sched->admit(pop_q(q));
    }
}
/* remove curr thread from scheduler and add to block thread */
void PROC_block_on(thread_q *q, int enable_ints) {
    thread curr;
    if (!q)
        return;
    curr = sched->active();
    sched->unlink(curr);
    push_q(q, curr);
    if (enable_ints)
        enable_interrupts();    
    yield(); 
}

void register_sched(scheduler new_sched) {
    if (sched != 0) {
        printk("Not Implemented: cannot reregister scheduler\n");
        // remove running procs and put them into new scheduler
        asm volatile("hlt");
    }
    sched=new_sched;
    sched->init();
}

void yield() {
    syscall(SYS_YIELD);
}

/* to yield*/
static void yield_sys() {
    thread next = 0, prev = 0;
    /* Invalid Call to Yield, or Nothing to Schedule*/
    if ((prev = sched->active()) == 0)
    {
        printk("Yield Error: Current is Null\n");
        asm volatile("hlt");
    }
    /* No Next, Exit */
    next = sched->next();
    /* If the calling thread is the same as next, just return immediately */
    if (next == prev) {
        return;
    }
    
    /* Magic */
    swap_rfiles(&(prev->state), &(next->state));
    return;
}

/* 
Terminate calling lwp, remove from active,
put in exited list and yield 
*/
void kexit() {
    asm volatile ("int $0x81");
}

void sys_exit() {
    thread prev = sched->active();
    kfree(prev->stack);
    sched->remove(prev);
    kfree(prev);
    swap_rfiles(0, &(sched->active()->state));
}

/* runs threads until all are exited */
void PROC_run() {
    thread main_thread = make_new_active_thread();
    /* Set Stack to null to designate main thread */
    main_thread -> stack = 0;
    if (sched->set_active(main_thread) != main_thread) {
        printk("error: proc failed to set active thread to main\n");
        asm volatile("hlt");
    };
    // if other things in scheduler, yield()
    while (sched->size() > 1) {
        yield();
    }
    printk("done\n");
    return;
}

/* create a thread with function entry_point and add it to active 
threads list, returns the new thread */
thread PROC_create_kthread(kproc_t entry_point, void *arg) {
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
    
    return new_thread;
}

/* 
Call the given lwpfunction with the given argument, then
calls lwp_exit() with its return value.
*/
static void lwp_wrapper(kproc_t fun, void *arg) {
    fun(arg); 
    kexit();
    return;
}

/*
mallocs a new thread and adds it to the active threads list
*/
static thread make_new_active_thread() {
    thread new_thread;
    new_thread = kmalloc(sizeof(context));
    new_thread -> tid = (uint64_t) new_thread;

    /* Add Thread to Scheduler */
    sched->admit(new_thread);
    return new_thread;
}

thread get_curr_thread() {
    return sched->active();
}