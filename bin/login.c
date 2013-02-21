#include <fs.h>
#include <pm.h>
#include <printf.h>

int main(int argc, char **argv)
{
  int child;
  char str[16];
  int r;

  open_std();

  printf("READY!\n");
  printf ("Going to fork in login!!\n");
  child = fork ();
  if (child == 0) {
    printf ("I am child!!!\n");
    exit (1);
  } else {
    printf ("I am parent!!\n");
  }
  waitpid (child, &r, 0);
  printf ("Child exited with value %d\n", r);
  r = read (0, str, 5);
  str[r] = '\0';
  printf("r = %d Result: %s\n", r, str);
  write(1, str, 5);

  return -1;
}

