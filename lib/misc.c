#include <misc.h>
#include <cpuinfo.h>
#include <config.h>

void wait_ready(void)
{
  cpuinfo->msg_ready[0] = true;
  while(! cpuinfo->ready)
    /* Wait */;
}

int check_server(void)
{
  if (cpuinfo->cpuid > NUMBER_SERVERS)
    return 0;

  return 1;
}
