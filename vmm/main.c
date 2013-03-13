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
static void
check_lapic_id(void)
{
  uint32_t ebx;
  uint8_t lapic_id;

  cpuid (0x1, NULL, &ebx, NULL, NULL);
  lapic_id = ebx >> 24;

  /* Double check */
  if (cpuinfo->cpuid != cpuinfo->lapic_id) {
    panic("Cpuid (%d) is different from lapic_id (%d)", cpuinfo->cpuid, cpuinfo->lapic_id);
  }

  if (lapic_id != cpuinfo->lapic_id) {
    panic("Real lapic ID (%d) is different from the one in cpuinfo (%d)", lapic_id, cpuinfo->lapic_id);
  }
}
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
  check_lapic_id();
  /* ================================== */
  vmx_init ();
  /* ================================== */
  panic ("VMM fatal: I MUST NEVER exit!!");
}
