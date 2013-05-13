#include <fs.h>
#include <pm.h>
#include <printf.h>
#include <string.h>
#include <asmfunc.h>
#include <debug.h>
#include <ipc.h>
#include <thread.h>
#include <gdt.h>
#include <interrupt.h>
#include <lapic.h>
static volatile int count = 0;
void
packet_handler (void)
{
  DEBUG ("\t\t\t%dth Packet was received!\n", 0x9, count);
  count++;
  lapic_eoi ();
}
int main(int argc, char **argv)
{
  create_default_gdt ();
  interrupt_init ();
  lapic_on ();    /* TODO: Check if its really needed */

  add_irq (32 + 18);

  sti ();

  for (;;) __asm__ __volatile__ ("hlt\n");
  return -1;
}

