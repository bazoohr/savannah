#ifndef __LIB_MEM_H__
#define __LIB_MEM_H__

#include <types.h>
#include <cpu.h>

phys_addr_t virt2phys (struct cpu_info *cpuinfo, virt_addr_t vaddr);

#endif /* __LIB_MEM_H__ */
