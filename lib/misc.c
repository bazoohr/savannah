#include <misc.h>
#include <cpuinfo.h>
#include <config.h>

void wait_ready(void)
{
  if (cpuinfo->cpuid == 0)
    return;

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

bool is_driver(cpuid_t id)
{
  return (id == KBD || id == CONSOLE) ? true : false;
}
