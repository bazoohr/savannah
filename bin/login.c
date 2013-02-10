#include <printk.h>
#include <console.h>

int main(int argc, char **argv)
{
  con_init();

  printk("Hello world! :-)\n");

  while (1){};
}

