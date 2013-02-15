#ifndef __I8259_H___
#define __I8259_H__

void i8259_init (void);
void i8259_eoi (void);
void i8259_enable (int irq);
void i8259_disable (int irq);

#endif  /* __I8259_H__ */
