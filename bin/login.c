#include <con.h>
#include <fs.h>

#include <printk.h>

int main(int argc, char **argv)
{
  char str[16];
  open("stdin", O_RDWR);
  int r = read (0, str, 5);
  str[r] = '\0';
  cprintk("\nr = %d Result: %s\n", 0xF, r, str);
  write(1, str, 5);
//  putc(str[0]);
//  putc('%');
//  putc(' ');
//  putc('l');
//  putc('s');

  while (1);
}

