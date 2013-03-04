#ifndef __CPU_CHECK_H__
#define __CPU_CHECK_H__

#include "asmfunc.h"

static inline bool
has_longmode (void)
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

static inline bool
has_x87_and_media_support (void)
{
  uint32_t eax;
  uint32_t ecx;
  uint32_t edx;

  cpuid (1, &eax, NULL, &ecx, &edx);

  if ((edx & (1 << 0))  &&  /* x87 */
      (edx & (1 << 25)) &&  /* sse */
      (edx & (1 << 26)) &&  /* sse2 */
      (ecx & (1 << 0))  &&  /* sse3 */
      (edx & (1 << 23)) &&  /* mmx */
      (edx & (1 << 24))     /* fxsave/fxstore */
     ) {
    return true;
  }
  return false;
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
has_1GBpage (void)
{
  uint32_t edx;

  cpuid (0x80000001, NULL, NULL, NULL, &edx);

  return (edx & (1 << 26)) ? true : false;
}
#endif /* __CPU_CHECK_H__ */
