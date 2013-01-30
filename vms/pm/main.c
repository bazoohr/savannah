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

// TODO Move this function in a general library
void
wait_ready(struct cpu_info *cpuinfo)
{
  cpuinfo->booted = 1;
  while(! cpuinfo->ready)
    /* Wait */;
}

// TODO Move this function in a general library
int
msg_receive(struct cpu_info *cpuinfo)
{
  int i;
  int from = -1;

  while (1) {
    for (i = 0 ; i < 8/*get_ncpus()*/ ; i++) {
      if (cpuinfo->msg_ready[i])
        from = i;
    }
    if (from != -1)
      break;
  }

  int m = cpuinfo->msg_output[from].data;

  cpuinfo->msg_ready[from] = false;

  return m;
}

void
vm_main (struct cpu_info *cpuinfo)
{
  wait_ready (cpuinfo);

  int i;
  con_init ();
  for (i = 0 ; i < cpuinfo->cpuid ; i++) printk("\n");

  cprintk ("PM: My info is in addr = %d\n", 0xD, cpuinfo->cpuid);

  int m = msg_receive(cpuinfo);

  cprintk("Message received: %d\n", 0xD, m);

  while (1) {__asm__ __volatile__ ("cli;pause;\n\t");}
}
