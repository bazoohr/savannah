#include <cdef.h>
#include <types.h>
#include <console.h>
#include <const.h>
#include <asmfunc.h>
#include <panic.h>
#include <ipc.h>
#include <string.h>
#include <cpuinfo.h>
#include <fs.h>
#include <debug.h>
#include <channel.h>
#include <vuos/vuos.h>
#include <stdlib.h>
/* ========================================== */
int
main (int argc, char **argv)
{
  struct read_char_rq req;
  int i;
  struct channel *from;
  void *write_data;

  int x;
  int y;

  if (argc < 3) {
    panic ("Too few arguments for console driver!");
  }
  x = atoi (argv[1]);
  y = atoi (argv[2]);

  con_init (x, y);

  while (1) {
    from = cnl_receive_any ();

    memcpy (&req, from->data, sizeof(struct read_char_rq));
    write_data = (void*)((phys_addr_t)from->data + sizeof(struct read_char_rq));

    for (i = 0 ; i < req.count ; i++) {
      putc (((char *)write_data)[i], 0xF);
    }

    cnl_reply(from, req.count);
  }

  for (;;);
  return 0;
}
