#include <cdef.h>
#include <types.h>
#include <console.h>
#include <const.h>
#include <asmfunc.h>
#include <panic.h>
#include <config.h>
#include <ipc.h>
#include <string.h>
#include <cpuinfo.h>
#include <fs.h>
#include <debug.h>
/* ========================================== */
int
main (int argc, char **argv)
{
  struct message *req;
  struct console_write cwrite;
  struct write_reply writereply;
  int i;
  int from;
  con_init ();

  while (1) {
    from = msg_receive(ANY);

    req = &cpuinfo->msg_input[from];
    memcpy (&cwrite, req->data, sizeof (struct console_write));

    for (i = 0 ; i < cwrite.count ; i++) {
      putc (((char *)cwrite.channel)[i], 0xF);
    }

    /*
     * cwrite.from MUST be equal to from, else
     * we are in a trouble
     */
    writereply.from = cwrite.from;
    writereply.count = cwrite.count;

    msg_send (FS, WRITE_ACK, &writereply, sizeof(struct write_reply));
    msg_receive (FS);
  }

  for (;;);
  return 0;
}
