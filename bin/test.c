#include <printf.h>
#include <fs.h>
#include <pm.h>

int main(int argc, char **argv)
{
  close (1);
  open ("stdout", O_RDWR);
  printf("Hello World\n");
  exit (1);
  return 0;
}
