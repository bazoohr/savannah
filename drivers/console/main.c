#include <cdef.h>
#include <dev/pic.h>
#include <types.h>
#include <printk.h>
#include <console.h>
#include <const.h>
#include <interrupt.h>
#include <asmfunc.h>
#include <dev/keyboard.h>
#include <dev/ioapic.h>
#include <dev/lapic.h>
#include <panic.h>
#include <config.h>
#include <ipc.h>
#include <con.h>
#include <string.h>
#include <cpuinfo.h>
/* ========================================== */
int
main (int argc, char **argv)
{
  con_init ();

  interrupt_init ();

  int i;
  for (i = 0 ; i < cpuinfo->cpuid ; i++) printk("\n");
  cprintk ("This is the console driver!!\n", 0xE);
  for (i = 0 ; i < argc ; i++) {
    cprintk ("argv[%d] = %s\n", 0xE, i, argv[i]);
  }

  while (1) {
    int from = msg_receive(ANY);

    struct putc_ipc tmp;

    memcpy(&tmp, cpuinfo->msg_input[from].data, sizeof(struct putc_ipc));

    kputc(tmp.c, 0xF);
  }

  for (;;);
  return 0;
}
