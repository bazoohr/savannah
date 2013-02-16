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
#include <cpuinfo.h>
#include <gdt.h>
#include <fs.h>
#include <ipc.h>
#include <string.h>
struct system_descriptor gdt[NGDT] __aligned (16);
/* ========================================== */
int
main (int argc, char **argv)
{
  struct message *req;
  struct keyboard_read kbd_rd;
  struct read_reply readreply;
  int i;

  con_init ();

  create_new_gdt (gdt, NGDT * sizeof (struct system_descriptor));
  interrupt_init ();

  /*
  pic_init ();
  ioapic_init ();
  lapic_init();
  kbd_init ();
  */

  for (i = 0 ; i < cpuinfo->cpuid ; i++) printk("\n");
  cprintk ("This is the keyboard driver!! %d\n", 0xE, argc);

  while (1) {
    msg_receive (FS);

    req = &cpuinfo->msg_input[FS];
    memcpy (&kbd_rd, req->data, sizeof (struct keyboard_read));

    cprintk ("request received from %d to read %d bytes into address = %x\n", 0x8, kbd_rd.from, kbd_rd.count, kbd_rd.channel);

    wait_for_completed_request(kbd_rd.channel, kbd_rd.count);

    asm volatile ("cli\n\r");

    readreply.from = kbd_rd.from;
    readreply.type = TYPE_CHAR;
    readreply.count = kbd_rd.count;
    readreply.channel = kbd_rd.channel;

    msg_send (FS, READ_ACK, &readreply, sizeof(struct read_reply));
  }

  for (;;);
  return 0;
}
