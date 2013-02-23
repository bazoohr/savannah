#include <fs.h>
#include <pm.h>
#include <printf.h>
#include <debug.h>
#include <cpuinfo.h>
#include <asmfunc.h>

int main(int argc, char **argv)
{
  int child;
  char str[16];
  int r;

  open_std();

  printf("READY!\n");
  /* ====================================== */
  printf ("Going to fork in login!!\n");
  child = fork ();
  if (child == 0) {
    printf ("This is the child with pid %d!\n", cpuinfo->cpuid);
    exit (1);
  } else {
    printf ("This is the parent with pid %d!\n", cpuinfo->cpuid);
  }
  /* ====================================== */
  waitpid (child, &r, 0);
  r = read (0, str, 5);
  printf ("r = %d\n", r);
  str[r] = '\0';
  printf("r = %d Result: %s\n", r, str);
  write(1, str, 5);

  return -1;
}

