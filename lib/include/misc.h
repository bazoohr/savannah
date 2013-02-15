#ifndef __MISC_H__
#define __MISC_H__

#include <types.h>

void wait_ready(void);
int check_server(void);
bool is_driver(cpuid_t id);

#endif /* __MISC_H__ */
