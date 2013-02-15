#include <con.h>
#include <fs.h>

int main(int argc, char **argv)
{
  char str[16];
  open("stdin", O_RDWR);
  read (0, str, 1);
  putc(str[0]);
  putc('%');
  putc(' ');
  putc('l');
  putc('s');

  while (1);
}

