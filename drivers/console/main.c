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

#if 0
  for (i = 0 ; i < cpuinfo->cpuid ; i++) printk("\n");
  cprintk ("This is the console driver!!\n", 0xE);
  for (i = 0 ; i < argc ; i++) {
    cprintk ("argv[%d] = %s\n", 0xE, i, argv[i]);
  }
#endif

  while (1) {
    from = msg_receive(ANY);

    req = &cpuinfo->msg_input[from];
    memcpy (&cwrite, req->data, sizeof (struct console_write));
    //DEBUG ("count = %d from %d %c\n", 0xA, cwrite.count, from, ((char*)cwrite.channel)[i]);

    for (i = 0 ; i < cwrite.count ; i++) {
    //  putc (i + '0', 0xA);
#if 0
      if (from == 5) {
        DEBUG ("%c ", 0xA, ((char*)cwrite.channel)[i]);
      }
#endif
      putc (((char *)cwrite.channel)[i], 0xF);
    }

    /*
     * cwrite.from MUST be equal to from, else
     * we are in a trouble
     */
    writereply.from = cwrite.from;
    writereply.count = cwrite.count;

    msg_send (FS, WRITE_ACK, &writereply, sizeof(struct write_reply));
//    struct putc_ipc tmp;
//
//    memcpy(&tmp, cpuinfo->msg_input[from].data, sizeof(struct putc_ipc));
//
//    putc(tmp.c, 0xF);
  }

  for (;;);
  return 0;
}
