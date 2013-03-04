#ifndef __ASMFUNC_H__
#define __ASMFUNC_H__

#include <types.h>

static inline void
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

static inline void
halt (void)
{
  for (;;) {
    __asm__ __volatile__ ("cli;hlt\n\t");
  }
}

#endif /* __ASMFUNC_H__ */
