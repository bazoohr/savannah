#ifndef __MP_H__
#define __MP_H__

#include <config.h>  /* For MAX & MIN number of cpus */

void mp_init(void);
void mp_bootothers (void);

#endif /* __MP_H__*/
