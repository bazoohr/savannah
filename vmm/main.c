#include <cdef.h>
#include <types.h>
#include <debug.h>
#include <const.h>
#include <interrupt.h>
#include <asmfunc.h>
#include <cpu.h>
#include <vmx.h>
#include <ipc.h>
#include <config.h>
#include <gdt.h>
#include <panic.h>
/* ========================================== */
struct system_descriptor gdt[NGDT] __aligned (16);
/* ========================================== */
void
vmm_main (void)
{
  create_new_gdt (gdt, NGDT * sizeof (struct system_descriptor));

  interrupt_init ();

  cpuinfo->ready = true;

  if (cpuinfo->cpuid != RS) {
    msg_receive (RS);
  }
  if (! cpuinfo->vmm_info.vmm_has_vm) {
    msg_receive (PM);
  }
  /* ================================== */
  vmx_init ();
  /* ================================== */
  panic ("VMM fatal: I MUST NEVER exit!!");
}
