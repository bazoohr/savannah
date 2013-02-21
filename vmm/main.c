#include <cdef.h>
#include <types.h>
#include <debug.h>
#include <console.h>
#include <const.h>
#include <interrupt.h>
#include <asmfunc.h>
#include <cpu.h>
#include <vmx.h>
#include <ipc.h>
#include <config.h>
#include <gdt.h>
/* ========================================== */
struct system_descriptor gdt[NGDT] __aligned (16);
/* ========================================== */
void
vmm_main (void)
{
  con_init ();
  create_new_gdt (gdt, NGDT * sizeof (struct system_descriptor));

  interrupt_init ();

  if (cpuinfo->vm_info.vm_start_vaddr == 0) {

    if (cpuinfo->vm_info.vm_start_paddr != 0) {
      DEBUG ("VMM[%d]: misconfigured VM information\n", 0x4, cpuinfo->cpuid);
      halt ();
    }

    cpuinfo->msg_ready[0] = true;

    while(! cpuinfo->ready);

    msg_receive (PM);
  }
  /* ================================== */
  vmx_init ();
  /* ================================== */
  DEBUG ("We should NEVER get here!\n", 0x4);
  halt ();
}
