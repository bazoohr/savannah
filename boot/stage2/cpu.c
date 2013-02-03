#include <types.h>
#include <printk.h>
#include <cpu.h>
#include <page.h>
#include <mp.h>
#include <memory.h>
#include <panic.h>
/* ================================================== */
static struct cpu_info *cpuinfo_table[MAX_CPUS];
/* ================================================== */
static uint64_t ncpus = 0;
/* ================================================== */
uint64_t
rdmsr (uint32_t ecx)
{
	register_t edx, eax;
	__asm__ __volatile__ ("rdmsr; mfence" : "=d"(edx), "=a"(eax) : "c"(ecx));
	return (uint64_t)((edx << 32) | eax);
}
/* ================================================== */
void
wrmsr (uint32_t reg, uint64_t val)
{
	__asm__ __volatile__ ("wrmsr" : : "d"((uint32_t)(val >> 32)),
	                         "a"((uint32_t)(val & 0xFFFFFFFF)),
	                         "c"(reg));
}
/* ================================================== */
void
lcr0 (uint64_t val)
{
	__asm __volatile__ ("movq %0,%%cr0" : : "r" (val));
}
/* ================================================== */
uint64_t
rcr0 (void)
{
	uint64_t val;
	__asm __volatile__ ("movq %%cr0,%0" : "=r" (val));
	return val;
}
/* ================================================== */
uint64_t
rcr2 (void)
{
	uint64_t val;
	__asm __volatile__ ("movq %%cr2,%0" : "=r" (val));
	return val;
}
/* ================================================== */
void
lcr3 (uint64_t val)
{
	__asm __volatile__ ("movq %0,%%cr3" : : "r" (val));
}
/* ================================================== */
uint64_t
rcr3 (void)
{
	uint64_t val;
	__asm __volatile__ ("movq %%cr3,%0" : "=r" (val));
	return val;
}
/* ================================================== */
void
lcr4 (uint64_t val)
{
	__asm __volatile__ ("movq %0,%%cr4" : : "r" (val));
}
/* ================================================== */
uint64_t
rcr4 (void)
{
	uint64_t cr4;
	__asm __volatile__ ("movq %%cr4,%0" : "=r" (cr4));
	return cr4;
}
/* ================================================== */
void
nop_pause (void)
{
    __asm __volatile__ ("pause" : : );
}
/* ================================================== */
void
cache_flush (void)
{
	__asm__ __volatile__ ("wbinvd");
}
/* ================================================== */
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
/* ================================================== */
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
/* ================================================== */
void __noreturn
halt(void)
{
	for (;;) {
    __asm__ __volatile__ ("cli;hlt\n");
  }
}
/* ================================================== */
uint64_t
get_ncpus (void)
{
  return ncpus;
}
/* ================================================== */
struct cpu_info *
get_cpu_info (cpuid_t cpuid)
{
  if (cpuid > MAX_CPUS) {
    panic ("get_cpu_info: No CPU with id %d\n", cpuid);
  }
  return cpuinfo_table[cpuid];
}
/* ================================================== */
struct cpu_info *
cpu_alloc (void)
{
  if (ncpus > MAX_CPUS) {
    panic ("Too many CPUs");
  }
  cpuinfo_table[ncpus] = (struct cpu_info *)calloc_align (sizeof (uint8_t), 0x1000, 0x1000);
  return cpuinfo_table[ncpus++];
}
