#ifndef __CPU_CHECK_H__
#define __CPU_CHECK_H__

#include "asmfunc.h"

static inline bool
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

static inline char *
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

static inline void
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

static inline bool
cpu_has_1GBpage (void)
{
  uint32_t edx;

  cpuid (0x80000001, NULL, NULL, NULL, &edx);

  return (edx & (1 << 26)) ? true : false;
}


static inline int
cpu_has_vmx (void)
{
	uint32_t ecx;
	cpuid (1, NULL, NULL, &ecx, NULL);
	return (ecx & (1 << 5));
}

static inline int
cpu_has_msr (void)
{
	uint32_t edx;
	cpuid (1, NULL, NULL, NULL, &edx);
	return (edx & (1 << 5));
}
static inline bool
cpu_has_physical_address_extention (void)
{
	uint32_t edx;
	cpuid (1, NULL, NULL, NULL, &edx);
	return (edx & (1 << 6));
}
#endif /* __CPU_CHECK_H__ */
