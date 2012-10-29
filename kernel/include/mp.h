#ifndef __MP_H__
#define __MP_H__

#define MIN_NUM_CPUS 4

void mp_init(void);
void mp_bootothers (void);

#endif /* __MP_H__*/
