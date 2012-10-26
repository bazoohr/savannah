#include <printk.h>
#include <printk.h>

/*
 * TODO
 *  Change it to include arguments.
 */
void
panic (const char *msg)
{
  cprintk ("PANIC %s\n", 0x4, msg);
  __asm__ __volatile__ ("cli;hlt\n\t");
}
