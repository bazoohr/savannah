#include <cdef.h>
#include <types.h>
#include <debug.h>
#include <const.h>
#include <interrupt.h>
#include <asmfunc.h>
#include <keyboard.h>
#include <panic.h>
#include <cpuinfo.h>
#include <gdt.h>
#include <fs.h>
#include <ipc.h>
#include <string.h>
#include <channel.h>
#include <vuos/vuos.h>
/* ========================================== */
void kbd_handler (void); /* Defined in isr.S */
/* ========================================== */
void
close_channel (struct channel *cnl)
{
  int reply_data;

  cnl_reply(cnl, 0);
  msg_receive(FS);

  reply_data = (int)cpuinfo->msg_input[FS].data[0];

  if (reply_data != 0) {
    panic ("KBD: The reply %d is wrong", reply_data);
  }
}
/* ========================================== */
int
main (int argc, char **argv)
{
  struct read_char_rq req;
  struct channel *from;

  create_default_gdt ();
  interrupt_init ();
  add_irq (33, &kbd_handler);

  cli ();

  while (1) {
    from = cnl_receive_any ();

    uint64_t *ptr = (void*)from->data;
    if (*ptr == CLOSE_CHANNEL) {
      close_channel(from);
      continue;
    }

    memcpy(&req, from->data, sizeof(struct read_char_rq));

    wait_for_completed_request((char*)from->data, req.count);

    cli ();

    cnl_reply(from, req.count);
  }

  for (;;);
  return 0;
}
