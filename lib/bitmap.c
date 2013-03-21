#include <bitmap.h>

void
set_bit(uint64_t *bitmap, const int bit)
{
	*bitmap |= (1 << bit);
}

void
clear_bit(uint64_t *bitmap, const int bit)
{
	*bitmap &= ~(1 << bit);
}

int
check_bit(const uint64_t *bitmap, const int bit)
{
	return ((*bitmap & (1 << bit)) >> bit);
}
