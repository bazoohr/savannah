#include <printf.h>
#include <fs.h>
#include <pm.h>
#include <timer.h>

static __inline int getpid (void)
{
  return cpuinfo->cpuid;
}
int main(int argc, char **argv)
{
  uint64_t b, a;
  int pid;

  b = get_cpu_cycle ();
  pid = getpid ();
  a = get_cpu_cycle ();
  printf ("getpid took %d cycles\n", a - b);
  pid++;

  b = get_cpu_cycle ();
  close (0);
  a = get_cpu_cycle ();
  printf ("close took %d cycles\n", a - b);

  b = get_cpu_cycle ();
  if (open ("stdin", O_RDWR) < 0) {
    printf ("failed to open!");
  }
  a = get_cpu_cycle ();
  printf ("open took %d cycles\n", a - b);

  b = get_cpu_cycle ();
  printf("Hello World\n");
  a = get_cpu_cycle ();
  printf ("printf took %d cycles\n", a - b);

  return 0;
}
