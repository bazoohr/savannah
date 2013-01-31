#include <misc.h>
#include <cpuinfo.h>
void wait_ready(void)
{
  cpuinfo->booted = 1;
  while(! cpuinfo->ready)
    /* Wait */;
}
