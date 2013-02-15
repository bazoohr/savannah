#include <cdef.h>
#include <types.h>
#include <printk.h>
#include <console.h>
#include <const.h>
#include <interrupt.h>
#include <asmfunc.h>
#include <cpu.h>
#include <vmx.h>
#include <ipc.h>
#include <gdt.h>
/* ========================================== */
struct system_descriptor gdt[NGDT] __aligned (16);
/* ========================================== */
void
vmm_main (void)
{
  int i;
  con_init ();
  create_new_gdt (gdt, NGDT * sizeof (struct system_descriptor));

  interrupt_init ();

  if (cpuinfo->vm_info.vm_start_vaddr == 0) {
    if (cpuinfo->vm_info.vm_start_paddr != 0) {
      cprintk ("VMM[%d]: misconfigured VM information\n", 0x4, cpuinfo->cpuid);
      halt ();
    }
    cpuinfo->msg_ready[0] = true;
    for (i = 0 ; i < cpuinfo->cpuid ; i++) cprintk("\n", 0x7);
    while(! cpuinfo->ready);
    cprintk ("IDLE: I am ready = %d\n", 0xA, cpuinfo->cpuid);
    /*
     * TODO:
     *      Wait for a message
     */
    msg_receive ();

    cprintk ("Going to launch rip = %x stack = %x\n", 0x2, cpuinfo->vm_info.vm_regs.rip, cpuinfo->vm_info.vm_regs.rsp);
    cprintk ("VM_INPUT %x VM_OUTPUT %x VM_READY = %x\n", 0x2, cpuinfo->msg_input, cpuinfo->msg_output, &cpuinfo->msg_ready[0]);
  }
  /* ================================== */
  vmx_init ();
  /* ================================== */
  cprintk ("We should NEVER get here!\n", 0x4);
  halt ();
}
