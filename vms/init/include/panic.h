#ifndef __PANIC_H__
#define __PANIC_H__

#include <printk.h>
#include <asmfunc.h>

#define panic(fmt...) \
do {printk (fmt); halt ();} while (0)

#endif /* __PANIC_H__ */
