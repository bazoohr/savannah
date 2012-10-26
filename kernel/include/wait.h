#ifndef __WAIT_H__
#define __WAIT_H__

#include <proc.h>
pid_t do_waitpid (const pid_t pid, int * const status);

#endif /* __WAIT_H__ */
