#include <dev/i8259.h>
#include <printk.h>

void
pic_init (void)
{
  i8259_init ();
}
void
pic_enable (int irq)
{
  i8259_enable (irq);
}
void
pic_disable (int irq)
{
  i8259_disable (irq);
}
void
pic_eoi (void)
{
  i8259_eoi ();
}
