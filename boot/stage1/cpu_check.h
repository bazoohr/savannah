#ifndef __CPU_CHECK_H__
#define __CPU_CHECK_H__

#include <cdef.h>
#include "asmfunc.h"

static __inline bool
cpu_has_longmode (void)
{
  uint32_t eax;
  uint32_t edx;

  cpuid (0x80000000, &eax, NULL, NULL, NULL);

  if (eax > 0x8000000) {
    cpuid (0x80000001, &eax, NULL, NULL, &edx);
    if (edx & (1 << 29))
      return true;
  }
  return false;
}

static __inline char *
cpu_has_x87_and_media_support (void)
{
  uint32_t eax;
  uint32_t ecx;
  uint32_t edx;

  cpuid (1, &eax, NULL, &ecx, &edx);

  if (! (edx & (1 << 0))) {
    return "ERROR: Your CPU does not support x87!";
  }
  if (!(edx & (1 << 25))) {
    return "ERROR: Your CPU does not support SSE!";
  }
  if (!(edx & (1 << 26))) {
    return "ERROR: Your CPU does not support SSE2!";
  }
  if (!(ecx & (1 << 0))) {
    return "ERROR: Your CPU does not support SSE3!";
  }
  if (!(ecx & (1 << 9))) {
    return "ERROR: Your CPU does not support SSSE3!";
  }
  if (!(ecx & (1 << 19))) {
    return "ERROR: Your CPU does not support SSE4.1!";
  }
  if (!(ecx & (1 << 20))) {
    return "ERROR: Your CPU does not support SSE4.2!";
  }
  if (!(edx & (1 << 23))) {
    return "ERROR: Your CPU does not support MMX!";
  }
  if (!(edx & (1 << 24))) {
    return "ERROR: Your CPU does not support fxsave/fxstore!";
  }
  return NULL;
}

static __inline void
enable_x87_and_media (void)
{
  __asm__ __volatile__ (
  /* let the processor know that we are going to use
   * media instructions, by setting cr4.osfxsr(bit number 9)*/
      "movl %cr4, %eax\n\t"
      "btsl $9, %eax\n\t"
      "movl %eax, %cr4\n\t"
  /* No processor emulate coprocessor
   * by clearing cr0.em (bit number 2) */
      "movl %cr0, %eax\n\t"
      "btrl $2, %eax\n\t"
      "movl %eax, %cr0\n\t"
  /* enable monitor coprocessor by enabling
   * cr0.mp (bit number 1) */
      "movl %cr0, %eax\n\t"
      "btsl $1, %eax\n\t"
      "movl %eax, %cr0\n\t"
      );
}

static __inline bool
cpu_has_1GBpage (void)
{
  uint32_t edx;

  cpuid (0x80000001, NULL, NULL, NULL, &edx);

  return (edx & (1 << 26)) ? true : false;
}


static __inline bool
cpu_has_vmx (void)
{
	uint32_t ecx;
	cpuid (1, NULL, NULL, &ecx, NULL);
	return (ecx & (1 << 5));
}

static __inline bool
cpu_has_msr (void)
{
	uint32_t edx;
	cpuid (1, NULL, NULL, NULL, &edx);
	return (edx & (1 << 5));
}
static __inline bool
cpu_has_physical_address_extention (void)
{
	uint32_t edx;
	cpuid (1, NULL, NULL, NULL, &edx);
	return (edx & (1 << 6));
}
static __inline bool
cpu_has_monitor_mwait (void)
{
  uint32_t ecx;

  cpuid (1, NULL, NULL, &ecx, NULL);

  return ecx & (1 << 3);
}
static __inline bool
cpu_has_rdtscp (void)
{
  uint32_t edx;

  cpuid (0x80000001, NULL, NULL, NULL, &edx);

  return edx & (1 << 27);
}
static __inline bool
cpu_has_perf_monitor (void)
{
  uint64_t misc_enable;

  misc_enable = rdmsr (0x1A0);
  printf ("%x misc_enable", misc_enable);
  return misc_enable & (1 << 7);
}

static __inline bool
cpu_cache_disable (void)
{
  uint32_t cr0 = rcr0 ();
  return cr0 & (1 << 30);
}

static __inline void
cpu_enable_cache (void)
{
  __asm__ __volatile__ (
      "movl %%cr0, %%eax\n\n"
      "btrl $29, %%eax\n\t"  /* Make sure NW is unset */
      "btrl $30, %%eax\n\t"  /* Unset CD bit */
      "movl %%eax, %%cr0\n\t"
      "wbinvd\n\t"
      :::"eax");
}
static __inline bool
cpu_has_rdrand (void)
{
  uint32_t ecx;

  cpuid (1, NULL, NULL, &ecx, NULL);

  return ecx & (1 << 30);
}
#endif /* __CPU_CHECK_H__ */
