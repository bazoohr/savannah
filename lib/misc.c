#include <misc.h>
#include <cpuinfo.h>
#include <vuos/vuos.h>

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
