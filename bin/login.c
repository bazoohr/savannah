#include <fs.h>
#include <printf.h>

int main(int argc, char **argv)
{
  char str[16];
  int r;

  open("stdin", O_RDWR);
  printf("READY!\n");
  r = read (0, str, 5);
  str[r] = '\0';
  printf("r = %d Result: %s\n", r, str);
  write(1, str, 5);
//  putc(str[0]);
//  putc('%');
//  putc(' ');
//  putc('l');
//  putc('s');

  while (1);
}

