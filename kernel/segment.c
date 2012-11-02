#include <string.h>
#include <segment.h>
#include <const.h>
#include <asmfunc.h>
#include <cdef.h>
#include <cpu.h>
#include <mp.h>
extern struct cpu cpus[MAX_CPUS];
#if 0
/* =========================================== */
struct system_descriptor gdt[NGDT] __aligned (16);
/* =========================================== */
void __inline
gdt_update_tss (struct system_descriptor *tss_desc)
{
	gdt[GDT_TSS] = *tss_desc;
}
/* =========================================== */
void __inline
gdt_update_ldt (struct system_descriptor *ldt_desc)
{
	gdt[GDT_LDT] = *ldt_desc;
}
/* =========================================== */
void
create_ldt_descriptor (struct ldt *ldt_addr, struct system_descriptor *ldt_desc)
{
  memset (ldt_desc, 0, sizeof (struct system_descriptor));

  ldt_desc->sd_lolimit = sizeof (struct ldt);
	ldt_desc->sd_lobase  = (phys_addr_t)ldt_addr;
	ldt_desc->sd_type    = SD_64LDT;
	ldt_desc->sd_dpl     = KNL_PVL;
	ldt_desc->sd_p       = 1;
	ldt_desc->sd_avl     = 1;
	ldt_desc->sd_G       = 1;
	ldt_desc->sd_hibase  = (phys_addr_t)ldt_addr >> 24;
}
/* =========================================== */
void
create_tss_descriptor (struct tss *tss_addr, struct system_descriptor *tss_desc)
{
  memset (tss_desc, 0, sizeof (struct system_descriptor));

  tss_desc->sd_lolimit = sizeof (struct tss);
	tss_desc->sd_lobase  = (phys_addr_t)tss_addr;
	tss_desc->sd_type    = SD_64TSS;
	tss_desc->sd_dpl     = KNL_PVL;
	tss_desc->sd_p       = 1;
	tss_desc->sd_avl     = 1;
	tss_desc->sd_G       = 1;
	tss_desc->sd_hibase  = (phys_addr_t)tss_addr >> 24;
}
/* =========================================== */
void
create_ldt (struct ldt *ldt, int dpl)
{
  struct code64_descriptor code64 = {
    .cd_three  = 3,
    .cd_dpl    = dpl,
    .cd_p      = 1,
    .cd_long   = 1,
    .cd_opndsz = 0,
  };
  struct data64_descriptor data64 = {
    .dd_w   = 1,
    .dd_one = 1,
    .dd_dpl = dpl,
    .dd_p   = 1,
  };
  ldt->code = code64;
  ldt->data = data64;
}
#endif
void
create_new_gdt (cpuid_t cpuid)
{
  struct descriptor_register gdtr;
  struct system_descriptor *gdt;

  struct code64_descriptor code64 = {
    .cd_three  = 3,
    .cd_dpl    = 0,
    .cd_p      = 1,
    .cd_long   = 1,
    .cd_opndsz = 0,
  };
  struct data64_descriptor data64 = {
    .dd_w   = 1,
    .dd_one = 1,
    .dd_p   = 1,
  };

  gdt = cpus[cpuid].gdt;

  memset (gdt, 0, NGDT * sizeof (struct system_descriptor));

  memcpy (&gdt[GDT_KCODE], &code64, sizeof code64);
  memcpy (&gdt[GDT_KDATA], &data64, sizeof data64);

  gdtr.dr_limit = NGDT * sizeof (struct system_descriptor) - 1;
  gdtr.dr_base  = (phys_addr_t)gdt;

  reload_gdt (&gdtr, KNL_CSEL, KNL_DSEL);
}
