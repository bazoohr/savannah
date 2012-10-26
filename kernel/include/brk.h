#ifndef __BRK_H__
#define __BRK_H__

#include <cdef.h>
void * __warn_unused_result do_sbrk (size_t size);
int __warn_unused_result do_brk (virt_addr_t addr);

#endif /* __BRK_H__ */
