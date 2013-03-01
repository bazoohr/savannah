#ifndef __CPUINFO_H__
#define __CPUINFO_H__

#include <types.h>
#include <cpu.h>

struct cpu_info * add_cpu (void);
inline int get_ncpus (void);
struct cpu_info * get_cpu_info (cpuid_t cpuid);

#endif /* __CPUINFO_H__ */
