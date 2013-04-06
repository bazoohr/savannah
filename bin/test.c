#include <printf.h>
#include <fs.h>
#include <pm.h>
#include <asmfunc.h>

static __inline int getpid (void)
{
  return cpuinfo->cpuid;
}
int main(int argc, char **argv)
{
  int pid;
  uint64_t a, b;
  unsigned int aux = cpuinfo->cpuid;

  b = rdtscp (&aux);
  pid = getpid ();
  a = rdtscp (&aux);
  printf ("getpid took %d cycles\n", a - b);
  pid++;

  b = rdtscp (&aux);
  close (0);
  a = rdtscp (&aux);
  printf ("close took %d cycles\n", a - b);

  b = rdtscp (&aux);
  if (open ("stdin", O_RDWR) < 0) {
    printf ("failed to open!");
  }
  a = rdtscp (&aux);
  printf ("open took %d cycles\n", a - b);

  b = rdtscp (&aux);
  printf("Hello World\n");
  a = rdtscp (&aux);
  printf ("printf took %d cycles\n", a - b);

  return 0;
}
