#include <printk.h>
#include <asmfunc.h>

/*
 * TODO
 *  Change it to include arguments.
 */
void
panic (const char *msg)
{
  cprintk ("PANIC %s\n", 0x4, msg);
  halt ();
}
