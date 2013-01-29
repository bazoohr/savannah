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
#include <string.h>

void
vm_main (struct cpu_info *cpuinfo)
{
  int i;
  con_init ();
  for (i = 0 ; i < cpuinfo->cpuid ; i++) cprintk("\n", 0x7);

  cpuinfo->booted = 1;
  while(! cpuinfo->ready);

  cprintk ("PM: My info is in addr = %d\n", 0xD, cpuinfo->cpuid);
  while (1) {
    if (cpuinfo->msg_box[cpuinfo->cpuid].from != 0 ||
        cpuinfo->msg_box[cpuinfo->cpuid].data != 0 ) {
      cprintk ("Message from %d data %x\n", 0xD, (cpuinfo->msg_box[cpuinfo->cpuid]).from, (cpuinfo->msg_box[cpuinfo->cpuid]).data);
      memset (&(cpuinfo->msg_box[cpuinfo->cpuid]), 0, sizeof (struct message));
    }
  }
  while (1) {__asm__ __volatile__ ("cli;pause;\n\t");}
}
