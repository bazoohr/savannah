#include <bitmap.h>

void __inline
set_bit(uint64_t *bitmap, const int bit)
{
	*bitmap |= (1 << bit);
}

void __inline
clear_bit(uint64_t *bitmap, const int bit)
{
	*bitmap &= ~(1 << bit);
}

int __inline
check_bit(const uint64_t *bitmap, const int bit)
{
	return ((*bitmap & (1 << bit)) >> bit);
}
