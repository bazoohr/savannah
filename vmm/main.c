#include <cdef.h>
#include <types.h>
#include <debug.h>
#include <const.h>
#include <interrupt.h>
#include <asmfunc.h>
#include <cpu.h>
#include <vmx.h>
#include <ipc.h>
#include <gdt.h>
#include <panic.h>
#include <vuos/config.h>
/* ========================================== */
void
vmm_main (void)
{
  create_default_gdt ();

  interrupt_init ();

  cpuinfo->ready = true;

  if (cpuinfo->cpuid != PM) {
    msg_receive (PM);
  }
  if (! cpuinfo->vmm_info.vmm_has_vm) {
    msg_receive (PM);
  }
  /* ================================== */
  vmx_init ();
  /* ================================== */
  panic ("VMM fatal: I MUST NEVER exit!!");
}
