#include <cdef.h>
#include <dev/pic.h>
#include <types.h>
#include <printk.h>
#include <console.h>
#include <const.h>
#include <interrupt.h>
#include <asmfunc.h>
#include <mp.h>
#include <cpu.h>
#include <vmx.h>
/* ========================================== */
void
vmm_main (struct cpu_info *cpuinfo)
{
  int i;
  con_init ();
  create_new_gdt (cpuinfo->cpuid);
  interrupt_init ();

  if (cpuinfo->vm_info.vm_start_vaddr == 0) {
    if (cpuinfo->vm_info.vm_start_paddr != 0) {
      cprintk ("VMM[%d]: misconfigured VM information\n", 0x4, cpuinfo->cpuid);
      halt ();
    }
    cpuinfo->booted = 1;
    for (i = 0 ; i < cpuinfo->cpuid ; i++) cprintk("\n", 0x7);
    while(! cpuinfo->ready);
    cprintk ("IDLE: I am ready = %d\n", 0xA, cpuinfo->cpuid);
    /*
     * TODO:
     *      Wait for a message
     */
    halt ();
  }
  /* ================================== */
  vmx_init (cpuinfo);
  /* ================================== */
  cprintk ("We should NEVER get here!\n", 0x4);
  halt ();
}
