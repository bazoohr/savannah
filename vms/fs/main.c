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
  int i;

  con_init ();
  for (i = 0 ; i < cpuinfo->cpuid; i++) cprintk("\n", 0x7);

  cpuinfo->booted = 1;
  while(! cpuinfo->ready)
    /* Wait */;

  cpuinfo->msg_box[0].from = 0;
  cpuinfo->msg_box[0].data = 0;

  cpuinfo->msg_box[0].from = cpuinfo->cpuid;
  cpuinfo->msg_box[0].data = 0xA;
  while (1) {__asm__ __volatile__ ("cli;pause;\n\t");}
}
