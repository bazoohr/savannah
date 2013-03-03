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
#include <channel.h>
/* ========================================== */
int
main (int argc, char **argv)
{
  struct read_char_rq req;
  int i;
  struct channel *from;
  char *write_data;

  int ch = 'f';

  while (1) {
    from = cnl_receive_any ();

    memcpy (&req, from->data, sizeof(struct read_char_rq));

    write_data = (char*)((phys_addr_t)from->data + sizeof(struct read_char_rq));

    for (i = 0 ; i < req.count ; i++) {
      write_data[i] = ch;
    }

    cnl_reply(from, req.count);
  }

  for (;;);
  return 0;
}
