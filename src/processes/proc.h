#ifndef PROC_H
#define PROC_H
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

/* Tuple that describes a scheduler */
typedef struct scheduler {
  void   (*init)(void);            /* initialize any structures                        */
  void   (*shutdown)(void);        /* tear down any structures                         */
  void   (*admit)(thread fresh);   /* add a thread to the pool                         */
  void   (*remove)(thread victim); /* remove a thread from the pool                    */
  thread (*next)(void);            /* set active to next thread and return that thread */
  thread (*active)(void);          /* current running thread                           */
  int (*size)(void);               /* num threads in scheduler                         */
  thread (*set_active)(thread);      /* set given thread to active thread, ret new active*/
} *scheduler;

/* 
yield execution of current running
proc, scheduler runs next proc 
*/
void yield(void);

/* 
set the scheduler for the threading lib
*/
void register_sched(scheduler new_sched);

/* 
destroys a process and returns 
associated memory 
*/
void kexit(void);
void sys_exit();
/* 
runs threads until all are exited, then returns
*/
void PROC_run(void);

// a kproc is a process that takes a void pointer and returns void
typedef void (*kproc_t)(void*);
/*
add thread to run
*/
thread PROC_create_kthread(kproc_t entry_point, void *arg);

/* must be called before using proc, registers syscall handlers */
void init_proc();

/* returns the current running thread */ 
thread get_curr_thread();
#endif