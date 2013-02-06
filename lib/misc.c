#include <misc.h>
#include <cpuinfo.h>
void wait_ready(void)
{
  cpuinfo->msg_ready[0] = true;
  while(! cpuinfo->ready)
    /* Wait */;
}
