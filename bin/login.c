#include <printk.h>
#include <console.h>

int main(int argc, char **argv)
{
  int i;
  con_init();
  for (i = 0; i < argc; i++) {
    printk("Hello world! %s :-)\n", argv[i]);
  }

  while (1){};
}

