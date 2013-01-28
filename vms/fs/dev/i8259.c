#include <asmfunc.h>
#include <printk.h>

/* 8259A interrupt controller values. */
#define INIT_SEQ        0x11	/* initialization sequance */
#define HW_INT1_BEGIN   0x20	/* start of hardware ints for 8259A-1 */
#define HW_INT2_BEGIN   0x28	/* start of hardware ints for 8259A-2 */
#define MASTER          0x04	/* 8259 is master */
#define SLAVE           0x02	/* 8259 is slave */
#define MODE_8086       0x01	/* 8259 is working in 8086 mode */
#define AUTO_EOI        0x02	/* 8259 automatic end of interrupt */
#define MASK_ALL        0xFF  /* 8259 mask all interrupts */
/* 8259A interrupt controller ports. */
#define INT_CTL         0x20	/* I/O port for interrupt controller */
#define INT_CTLMASK     0x21	/* setting bits in this port disables ints */
#define INT2_CTL        0xA0	/* I/O port for second interrupt controller */
#define INT2_CTLMASK    0xA1	/* setting bits in this port disables ints */

#define EOI 0x20

static uint16_t i8259_mask;

static void
i8259_setmask (uint16_t mask)
{
  if (i8259_mask != mask) {
	  outb (mask, INT_CTLMASK);        /* mask off all hw ints 8259A-1 */
	  outb (mask >> 8, INT2_CTLMASK);  /* mask off all hw ints 8259A-2 */

    i8259_mask = mask;
  }
}

void
i8259_init (void)
{
  i8259_mask = MASK_ALL;

  outb (INIT_SEQ, INT_CTL);  /* 8259A-1 */
  outb (INIT_SEQ, INT2_CTL); /* 8259A-2 */

	outb (HW_INT1_BEGIN, INT_CTLMASK);  /* start of hw ints for 8259A-1 */
	outb (HW_INT2_BEGIN, INT2_CTLMASK); /* start of hw ints for 8259A-2 */

	outb (MASTER, INT_CTLMASK); /* 8259A-1 is master */
	outb (SLAVE, INT2_CTLMASK); /* 8259A-2 is slave */

	outb (MODE_8086/* | AUTO_EOI*/, INT_CTLMASK);   /* 8259A-1, 8086 mode + auto EOI */
	outb (MODE_8086/* | AUTO_EOI*/, INT2_CTLMASK);  /* 8259A-2, 8086 mode + auto EOI */

	outb (i8259_mask, INT_CTLMASK);        /* mask off all hw ints 8259A-1 */
	outb (i8259_mask >> 8, INT2_CTLMASK);  /* mask off all hw ints 8259A-2 */
}

void
i8259_eoi (void)
{
  outb (EOI, INT_CTL);
  outb (EOI, INT2_CTL);
}

void
i8259_enable (int irq)
{
  i8259_setmask (i8259_mask & ~(1 << irq));
}

void
i8259_disable (int irq)
{
  i8259_setmask (i8259_mask | (1 << irq));
}
