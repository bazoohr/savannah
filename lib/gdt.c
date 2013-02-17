#include <string.h>
#include <const.h>
#include <cdef.h>
#include <cpu.h>
#include <cpuinfo.h>
#include <gdt.h>

void
create_new_gdt (struct system_descriptor *gdt, size_t sizeof_gdt)
{
  struct descriptor_register gdtr;

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

  reload_gdt (&gdtr, KNL_CSEL, KNL_DSEL);
}
