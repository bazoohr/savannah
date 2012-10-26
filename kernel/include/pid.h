#ifndef __PID_H__
#define __PID_H__

#include <types.h>

typedef int pid_t;

void pm_init (void);
pid_t alloc_pid (void);
void free_pid (pid_t pid);
struct proc *get_pcb (pid_t pid);
struct proc *alloc_pcb (pid_t pid);

#endif /* __PID_H__ */
