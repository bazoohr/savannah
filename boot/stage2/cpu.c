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
static bool first_time = true;
static struct cpu_info *cpuinfo_pool;
/* ================================================== */
void
allocate_cpuinfo_pool (void)
{
  uint64_t max_pool_size = MAX_CPUS * _4KB_;

  if (sizeof (struct cpu_info) > _4KB_) {
    panic ("CPU info structure is too big. 4KB structure is accepted but the size is %d!\n", sizeof (struct cpu_info));
  }

  if (max_pool_size > _2MB_) {
    panic ("Second Boot Stage: Max CPU information Pool is bigger than 2MB\n");
  }

  cpuinfo_pool = (struct cpu_info *)calloc_align (sizeof (uint8_t), _2MB_, _2MB_);
  /* We put the first CPU in cpuinfo table */
  cpuinfo_table[0] = cpuinfo_pool;
}
/* ================================================== */
struct cpu_info *
add_cpu ()
{
  if (ncpus > MAX_CPUS) {
    panic ("Too many CPUs");
  }

  if (first_time) {
    first_time = false;
    allocate_cpuinfo_pool ();
  }

  cpuinfo_table[ncpus] = (struct cpu_info *)((phys_addr_t)cpuinfo_pool + (ncpus * _4KB_));
  return cpuinfo_table[ncpus++];
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
