#ifndef __STDLIB_H__
#define __STDLIB_H__

#include <types.h>

static inline bool isxdigit (char c)
{
  return ('0' <= (c) && (c) <= '9') ||
         ('a' <= (c) && (c) <= 'f') ||
         ('A' <= (c) && (c) <= 'F');
}

static inline bool isdigit (char c)
{
 return  '0' <= c && c <= '9';
}

static inline bool islower (char c)
{
  return 'a' <= c && c <= 'z';
}

static inline char toupper (char c)
{
  return islower (c) ? c - 'a' + 'A' : c;
}

int atoi (const char *s);

#endif /* __STDLIB_H__ */
