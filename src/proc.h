#ifndef PROC_H
#define PROC_H

/* 
yield execution of current running
proc, scheduler runs next proc 
*/
void yield(void);

/* 
destroys a process and returns 
associated memory 
*/
void exit(void);

/* 
runs threads until all are exited, then returns
*/
void PROC_run(void);

// a kproc is a process that takes a void pointer and returns void
typedef void (*kproc_t)(void*);
/*
add thread to run
*/
void PROC_create_kthread(kproc_t entry_point, void *arg);

#endif