#include <cdef.h>
#include <dev/pic.h>
#include <types.h>
#include <debug.h>
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
#include <channel.h>
/* ========================================== */
int
main (int argc, char **argv)
{
  struct read_char_rq req;
  struct channel *from;

  create_default_gdt ();
  interrupt_init ();

  cli ();

  while (1) {
    from = cnl_receive_any ();

    memcpy(&req, from->data, sizeof(struct read_char_rq));

    wait_for_completed_request((char*)from->data, req.count);

    cli ();

    cnl_reply(from, req.count);
  }

  for (;;);
  return 0;
}
