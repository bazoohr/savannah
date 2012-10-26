#ifndef __LOCK_H__
#define __LOCK_H__

#ifndef __ASSEMBLY__
void lock_region (int *lock);
void unlock_region (int *lock);
#endif

#define REGION_LOCKED      1
#define REGION_NOT_LOCKED  0
#endif /* __LOCK_H__ */
