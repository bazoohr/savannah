#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <printk.h>

#define DEBUG(fmt, color, args...) cprintk ("%s->%s: "fmt, color, __FILE__, __func__, args);

#endif /* __DEBUG_H__ */
