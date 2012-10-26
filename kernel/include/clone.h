#ifndef __CLONE_H__
#define __CLONE_H__

#include <frame.h>
#include <cdef.h>

void do_clone (struct trapframe *tf);
void do_unclone (void __unused *status);

#endif /* __CLONE_H__ */
