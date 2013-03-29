#ifndef __ASMFUNC_H__
#define __ASMFUNC_H__

#include <types.h>
#include <cdef.h>

static __inline void
cpuid (uint32_t info, uint32_t *eaxp, uint32_t *ebxp,
       uint32_t *ecxp, uint32_t *edxp)
{
	uint32_t eax, ebx, ecx, edx;
	__asm__ __volatile__ ("cpuid"
		: "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
		: "a" (info));
	if (eaxp)
		*eaxp = eax;
	if (ebxp)
		*ebxp = ebx;
	if (ecxp)
		*ecxp = ecx;
	if (edxp)
		*edxp = edx;
}

static __inline uint64_t
rdmsr (uint32_t reg)
{
	uint32_t edx, eax;
	__asm__ __volatile__ ("rdmsr; mfence" : "=d"(edx), "=a"(eax) : "c"(reg));
	return (register_t)edx << 32 | eax;
}

static __inline void
lcr0 (uint32_t val)
{
	__asm __volatile__ ("movl %0,%%cr0" : : "r" (val));
}
static __inline uint32_t
rcr0 (void)
{
	uint32_t val;
	__asm __volatile__ ("movl %%cr0,%0" : "=r" (val));
	return val;
}
static __inline void
halt (void)
{
  for (;;) {
    __asm__ __volatile__ ("cli;hlt\n\t");
  }
}

#endif /* __ASMFUNC_H__ */
