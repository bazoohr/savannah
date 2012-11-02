#include <types.h>
#include <cpu.h>
#include <page.h>
uint64_t
rdmsr (uint32_t reg)
{
	register_t edx, eax;
	__asm__ __volatile__ ("rdmsr; mfence" : "=d"(edx), "=a"(eax) : "c"(reg));
	return (register_t)edx << 32 | eax;
}

void
wrmsr (uint32_t reg, uint64_t val)
{
	__asm__ __volatile__ ("wrmsr" : : "d"((uint32_t)(val >> 32)),
	                         "a"((uint32_t)(val & 0xFFFFFFFF)), 
	                         "c"(reg));
}
void
lcr0 (uint64_t val)
{
	__asm __volatile__ ("movq %0,%%cr0" : : "r" (val));
}

uint64_t
rcr0 (void)
{
	uint64_t val;
	__asm __volatile__ ("movq %%cr0,%0" : "=r" (val));
	return val;
}

uint64_t
rcr2 (void)
{
	uint64_t val;
	__asm __volatile__ ("movq %%cr2,%0" : "=r" (val));
	return val;
}

void
lcr3 (uint64_t val)
{
	__asm __volatile__ ("movq %0,%%cr3" : : "r" (val));
}

uint64_t
rcr3 (void)
{
	uint64_t val;
	__asm __volatile__ ("movq %%cr3,%0" : "=r" (val));
	return val;
}

void
lcr4 (uint64_t val)
{
	__asm __volatile__ ("movq %0,%%cr4" : : "r" (val));
}

uint64_t
rcr4 (void)
{
	uint64_t cr4;
	__asm __volatile__ ("movq %%cr4,%0" : "=r" (cr4));
	return cr4;
}

void
nop_pause (void)
{
    __asm __volatile__ ("pause" : : );
}

void
cache_flush (void)
{
	__asm__ __volatile__ ("wbinvd");
}

/* Flushes a TLB, including global pages.  We should always have the CR4_PGE
 * flag set, but just in case, we'll check.  Toggling this bit flushes the TLB.
 */
void
tlb_flush_global (void)
{
	uint64_t cr4 = rcr4();
	if (cr4 & CR4_PGE) {
		lcr4(cr4 & ~CR4_PGE);
		lcr4(cr4);
	} else 
		lcr3(rcr3());
}
void
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

