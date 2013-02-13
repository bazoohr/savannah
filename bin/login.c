#include <printk.h>
#include <console.h>

int main(int argc, char **argv)
{
  int i;
  con_init();
  for (i = 0; i < argc; i++) {
    cprintk("Hello world! %s :-)\n", 0x3, argv[i]);
  }

  while (1){};
}

