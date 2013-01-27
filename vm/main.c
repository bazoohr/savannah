#include <cdef.h>
#include <dev/pic.h>
#include <types.h>
#include <kernel_args.h>
#include <printk.h>
#include <console.h>
#include <const.h>
#include <interrupt.h>
#include <asmfunc.h>
#include <memory.h>
#include <dev/keyboard.h>
#include <mp.h>
#include <dev/ioapic.h>
#include <dev/lapic.h>
#include <cpu.h>

void
vm_main (struct cpu_info *cpuinfo)
{
  con_init ();
  cprintk ("My info is in addr = %x\n", 0xA, cpuinfo->cpuid);
  while (1) {__asm__ __volatile__ ("cli;pause;\n\t");}
}
