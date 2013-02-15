#ifndef __CON_H__
#define __CON_H__

#include <fs.h>

struct putc_ipc {
	int f;
	int c;
};

int putc(int c);

#endif /* __CON_H__ */
