#ifndef __ASMFUNC_H__
#define __ASMFUNC_H__

#include <cdef.h>
#include <types.h>
#include <cpu.h>

static __inline void
cli (void)
{
  __asm__ __volatile__ ("cli\n");
}
static __inline void
sti (void)
{
  __asm__ __volatile__ ("sti\n");
}

static __inline uint8_t
inb (uint16_t port)
{
  uint8_t  data;
  __asm __volatile("inb %%dx,%0" : "=a" (data) : "d" (port));
  return data;
}

static __inline void
outb (uint8_t data, uint16_t port)
{
    __asm __volatile("outb %0,%%dx" : : "a" (data), "d" (port));
}

static __inline void
sidt (struct descriptor_register *dtr)
{
  __asm__ __volatile__ ("sidt %0":"=m" (*dtr));
}

static __inline void
lidt (const struct descriptor_register *dtr)
{
  __asm__ __volatile__ ("lidt %0"::"m" (*dtr));
}

static __inline void
sgdt (struct descriptor_register *dtr)
{
  __asm__ __volatile__ ("sgdt %0":"=m" (*dtr));
}

static __inline uint64_t
rdmsr (uint32_t reg)
{
	register_t edx, eax;
	__asm__ __volatile__ ("rdmsr; mfence" : "=d"(edx), "=a"(eax) : "c"(reg));
	return (register_t)edx << 32 | eax;
}

static __inline void
wrmsr (uint32_t reg, uint64_t val)
{
	__asm__ __volatile__ ("wrmsr" : : "d"((uint32_t)(val >> 32)),
                         "a"((uint32_t)(val & 0xFFFFFFFF)),
	                         "c"(reg));
}
static __inline void
lcr0 (uint64_t val)
{
	__asm __volatile__ ("movq %0,%%cr0" : : "r" (val));
}

static __inline uint64_t
rcr0 (void)
{
	uint64_t val;
	__asm __volatile__ ("movq %%cr0,%0" : "=r" (val));
	return val;
}

static __inline uint64_t
rcr2 (void)
{
	uint64_t val;
	__asm __volatile__ ("movq %%cr2,%0" : "=r" (val));
	return val;
}

static __inline void
lcr3 (uint64_t val)
{
	__asm __volatile__ ("movq %0,%%cr3" : : "r" (val));
}

static __inline uint64_t
rcr3 (void)
{
	uint64_t val;
	__asm __volatile__ ("movq %%cr3,%0" : "=r" (val));
	return val;
}

static __inline void
lcr4 (uint64_t val)
{
	__asm __volatile__ ("movq %0,%%cr4" : : "r" (val));
}

static __inline uint64_t
rcr4 (void)
{
	uint64_t cr4;
	__asm __volatile__ ("movq %%cr4,%0" : "=r" (cr4));
	return cr4;
}

static __inline void
nop_pause (void)
{
    __asm __volatile__ ("pause" : : );
}

static __inline void
cache_flush (void)
{
	__asm__ __volatile__ ("wbinvd");
}

static __inline void
cpuid (uint32_t function, uint32_t *eaxp, uint32_t *ebxp,
      uint32_t *ecxp, uint32_t *edxp)
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

static __inline void __noreturn
halt(void)
{
	for (;;) {
    __asm__ __volatile__ ("cli;hlt\n");
  }
}

#endif /* __ASMFUNC_H__ */
