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

// TODO Move this function in a general library
void
wait_ready(struct cpu_info *cpuinfo)
{
  cpuinfo->booted = 1;
  while(! cpuinfo->ready)
    /* Wait */;
}

// TODO Move this function in a general library
void
msg_send(struct cpu_info *cpuinfo, const int to, const int data)
{
  cpuinfo->msg_output[cpuinfo->cpuid].from = cpuinfo->cpuid;
  cpuinfo->msg_output[cpuinfo->cpuid].data = data;
  cpuinfo->msg_ready[cpuinfo->cpuid] = true;
}

void
vm_main (struct cpu_info *cpuinfo)
{
  wait_ready(cpuinfo);

  int i;
  con_init ();
  for (i = 0 ; i < cpuinfo->cpuid; i++) printk("\n");

  cprintk ("FS: My info is in addr = %d\n", 0xD, cpuinfo->cpuid);

  msg_send(cpuinfo, 0, 42);

  while (1) {__asm__ __volatile__ ("cli;pause;\n\t");}
}
