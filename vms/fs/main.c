#include <cdef.h>
#include <dev/pic.h>
#include <types.h>
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
  int i;
  for (i = 0 ; i < cpuinfo->cpuid ; i++) cprintk("\n", 0x7);
  cprintk ("FS: My info is in addr = %d\n", 0xA, cpuinfo->cpuid);
  cpuinfo->booted = 1;
  while (1) {__asm__ __volatile__ ("cli;pause;\n\t");}
}
