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
#include <cpuinfo.h>
#include <fs.h>
#include <string.h>

void
vm_main (void)
{
  int i;
  con_init ();
  for (i = 0 ; i < cpuinfo->cpuid ; i++) printk("\n");

  cprintk ("PM: My info is in addr = %d\n", 0xD, cpuinfo->cpuid);

  while (1) {__asm__ __volatile__ ("cli;pause;\n\t");}
}
