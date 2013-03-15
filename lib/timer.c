#include <timer.h>
#include <asmfunc.h>

uint64_t
get_cpu_cycle (void)
{
  return rdtsc ();
}
