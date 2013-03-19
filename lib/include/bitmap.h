#ifndef __BITMAP_H__
#define __BITMAP_H__

#include <types.h>

void __inline set_bit(uint64_t *bitmap, const int bit);
void __inline clear_bit(uint64_t *bitmap, const int bit);
int __inline check_bit(const uint64_t *bitmap, const int bit);

#endif /* __BITMAP_H__ */
