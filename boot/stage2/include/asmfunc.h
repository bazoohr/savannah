#ifndef __ASMFUNC_H__
#define __ASMFUNC_H__

#include <types.h>
#include <cdef.h>
#include <cpu.h>

static inline uint64_t rdmsr (uint32_t ecx)
{
	register_t edx, eax;
	__asm__ __volatile__ ("rdmsr; mfence" : "=d"(edx), "=a"(eax) : "c"(ecx));
	return (uint64_t)((edx << 32) | eax);
}
/* ================================================== */
static inline void wrmsr (uint32_t reg, uint64_t val)
{
	__asm__ __volatile__ ("wrmsr" : : "d"((uint32_t)(val >> 32)),
	                         "a"((uint32_t)(val & 0xFFFFFFFF)),
	                         "c"(reg));
}
/* ================================================== */
static inline uint8_t
inb (uint16_t port)
{
  uint8_t  data;
  __asm __volatile("inb %%dx,%0" : "=a" (data) : "d" (port));
  return data;
}
/* ================================================== */
static inline void
outb (uint8_t data, uint16_t port)
{
    __asm __volatile("outb %0,%%dx" : : "a" (data), "d" (port));
}
/* ================================================== */
static void inline lcr0 (uint64_t val)
{
	__asm __volatile__ ("movq %0,%%cr0" : : "r" (val));
}
/* ================================================== */
static inline void
lidt (const struct descriptor_register *dtr)
{
  __asm__ __volatile__ ("lidt %0"::"m" (*dtr));
}
/* ================================================== */
static inline uint64_t rcr0 (void)
{
	uint64_t val;
	__asm __volatile__ ("movq %%cr0,%0" : "=r" (val));
	return val;
}
/* ================================================== */
static inline uint64_t rcr2 (void)
{
	uint64_t val;
	__asm __volatile__ ("movq %%cr2,%0" : "=r" (val));
	return val;
}
/* ================================================== */
static inline void lcr3 (uint64_t val)
{
	__asm __volatile__ ("movq %0,%%cr3" : : "r" (val));
}
/* ================================================== */
static inline uint64_t rcr3 (void)
{
	uint64_t val;
	__asm __volatile__ ("movq %%cr3,%0" : "=r" (val));
	return val;
}
/* ================================================== */
static inline void lcr4 (uint64_t val)
{
	__asm __volatile__ ("movq %0,%%cr4" : : "r" (val));
}
/* ================================================== */
static inline uint64_t rcr4 (void)
{
	uint64_t cr4;
	__asm __volatile__ ("movq %%cr4,%0" : "=r" (cr4));
	return cr4;
}
/* ================================================== */
static inline void nop_pause (void)
{
    __asm __volatile__ ("pause" : : );
}
/* ================================================== */
static inline void cache_flush (void)
{
	__asm__ __volatile__ ("wbinvd");
}
/* ================================================== */
/* Flushes a TLB, including global pages.  We should always have the CR4_PGE
 * flag set, but just in case, we'll check.  Toggling this bit flushes the TLB.
 */
static inline void tlb_flush_global (void)
{
	uint64_t cr4 = rcr4();
	if (cr4 & CR4_PGE) {
		lcr4(cr4 & ~CR4_PGE);
		lcr4(cr4);
	} else
		lcr3(rcr3());
}
/* ================================================== */
static inline void
cli (void)
{
  __asm__ __volatile__ ("cli;\n\t");
}
/* ================================================== */
static inline void cpuid (uint32_t function,
                          uint32_t *eaxp,
                          uint32_t *ebxp,
                          uint32_t *ecxp,
                          uint32_t *edxp)
{
	uint32_t eax, ebx, ecx, edx;

	__asm__ __volatile__ ("cpuid"
		: "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
		: "a" (function));
	if (eaxp)
		*eaxp = eax;
	if (ebxp)
		*ebxp = ebx;
	if (ecxp)
		*ecxp = ecx;
	if (edxp)
		*edxp = edx;
}
/* ================================================== */
static inline void __noreturn
halt(void)
{
	for (;;) {
    __asm__ __volatile__ ("cli;hlt\n");
  }
}
#endif /* __ASMFUNC_H___ */
