#ifndef __PROC_H__
#define __PROC_H__

#include <cdef.h>
typedef int pid_t;
typedef pid_t tid_t;

pid_t __inline fork (void);
void __inline  exit (int status);
pid_t __inline waitpid (pid_t pid, int *status);
#endif
