#ifndef __PIC_H__
#define __PIC_H__

#ifndef __ASSEMBLY__
void pic_init (void);
void pic_eoi (void);
void pic_enable (int irq);
void pic_disable (int irq);
#endif /* __ASSEMBLY__ */

#endif
