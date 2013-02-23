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
  //volatile int i = 0;
  int r;

  open_std();

  //printf("READY!\n");
  /* ====================================== */
  //printf ("Going to fork in login!!\n");
  child = fork ();
  if (child == 0) {
    //for (i = 0; i < 999999; i++);
    for (;;) {
      printf ("%c", 'C');
    }
#if 0
    printf ("\nParent cpuid %d\n", cpuinfo->cpuid);
    printf ("\nParent type %d\n", cpuinfo->vm_info.fds[1].type);
    printf ("\nParent offset %x\n", cpuinfo->vm_info.fds[1].offset);
    printf ("\nParent dst %d\n", cpuinfo->vm_info.fds[1].dst);
    printf ("\nParent length  %d\n", cpuinfo->vm_info.fds[1].length);
#endif
    exit (1);
  } else {
    for (;;) {
      printf ("%c", 'P');
    }
  }
  /* ====================================== */
  waitpid (child, &r, 0);
  r = read (0, str, 5);
  DEBUG ("r = %d\n", 0x4, r);
  exit (2);
  str[r] = '\0';
  printf("r = %d Result: %s\n", r, str);
  write(1, str, 5);

  return -1;
}

