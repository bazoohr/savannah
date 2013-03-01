#include <string.h>
#include <gdt.h>
#include <const.h>
#include <asmfunc.h>
#include <cdef.h>
#include <cpu.h>
#include <mp.h>
/* ======================================== */
void
create_default_gdt (void)
{
  size_t sizeof_gdt;
  static struct system_descriptor gdt[NGDT] __aligned (16);
  /*
   * Since we only use gdtr in assembly code, without "volatile" GCC may
   * remove all codes regarding gdtr assuming them as unused. So
   * for the code to work perfectly is all optimization levels, we need
   * to use volatile here.
   */
  volatile struct descriptor_register gdtr __aligned (0x10);

  struct code64_descriptor code64 __aligned (0x10) = {
    .cd_three  = 3,
    .cd_dpl    = 0,
    .cd_p      = 1,
    .cd_long   = 1,
    .cd_opndsz = 0,
  };
  struct data64_descriptor data64 __aligned (0x10) = {
    .dd_w   = 1,
    .dd_one = 1,
    .dd_p   = 1,
  };

  sizeof_gdt = NGDT * sizeof (struct system_descriptor);

  memset (gdt, 0, sizeof_gdt);

  memcpy (&gdt[GDT_KCODE], &code64, sizeof (struct code64_descriptor));
  memcpy (&gdt[GDT_KDATA], &data64, sizeof (struct data64_descriptor));

  /*
   *  Size of the GDT in bytes. The limit value is added
   *  to the base address to yield the ending byte address of the GDT.
   */
  gdtr.dr_limit = sizeof_gdt - 1;
  /*
   * The base-address field holds the starting byte address of the GDT in virtual-
   * memory space.
   */
  gdtr.dr_base  = (phys_addr_t)gdt;
  /* Reloading GDT */
  __asm__ __volatile__ (
      "cli\n\t"
      "lgdtq (%0)\n\t"
      "movl %1, %%ds\n\t"
      "movl %1, %%es\n\t"
      "movl %1, %%fs\n\t"
      "movl %1, %%gs\n\t"
      "movl %1, %%ss\n\t"
      "pushq %2\n\t"
      "pushq $1f\n\t"
      "lretq\n\t"
      "1:\n\t"
      ::"r"((virt_addr_t)&gdtr), "r"(KNL_DSEL), "I"(KNL_CSEL)
  );
}
