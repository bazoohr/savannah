#ifndef __MP_H__
#define __MP_H__

#define MIN_CPUS 4
#define MAX_CPUS 255

void mp_init(void);
void mp_bootothers (void);

#endif /* __MP_H__*/
