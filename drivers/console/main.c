#include <cdef.h>
#include <types.h>
#include <printk.h>
#include <console.h>
#include <const.h>
#include <asmfunc.h>
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
  struct message *req;
  struct console_write cwrite;
  struct write_reply writereply;
  int i;
  con_init ();

  for (i = 0 ; i < cpuinfo->cpuid ; i++) printk("\n");
  cprintk ("This is the console driver!!\n", 0xE);
  for (i = 0 ; i < argc ; i++) {
    cprintk ("argv[%d] = %s\n", 0xE, i, argv[i]);
  }

  while (1) {
    msg_receive(FS);

    req = &cpuinfo->msg_input[FS];
    memcpy (&cwrite, req->data, sizeof (struct console_write));

    for (i = 0 ; i < cwrite.count ; i++) {
      kputc(((char*)cwrite.channel)[i], 0xF);
    }

    writereply.from = cwrite.from;
    writereply.count = cwrite.count;

    msg_send (FS, WRITE_ACK, &writereply, sizeof(struct write_reply));
//    struct putc_ipc tmp;
//
//    memcpy(&tmp, cpuinfo->msg_input[from].data, sizeof(struct putc_ipc));
//
//    kputc(tmp.c, 0xF);
  }

  for (;;);
  return 0;
}
