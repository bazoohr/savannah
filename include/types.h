#ifndef __TYPES_H
#define __TYPES_H

#ifndef NULL
#define NULL  ((void*)0)
#endif

typedef unsigned char		        uint8_t;
typedef unsigned short int	    uint16_t;
typedef unsigned int		        uint32_t;
#ifdef __x86_64__
typedef long int		  int64_t;
#else
typedef long long int int64_t;
#endif

typedef signed char		int8_t;
typedef short int		  int16_t;
typedef int			      int32_t;
#if __x86_64__
typedef unsigned long int	      uint64_t;
#else
typedef unsigned long long int	uint64_t;
#endif

/* Types for `void *' pointers.  */
#ifdef __x86_64
typedef long int		      intptr_t;
typedef unsigned long int	uintptr_t;
#else
typedef int		         	intptr_t;
typedef unsigned int		uintptr_t;
#endif

#ifdef __x86_64__
typedef uint64_t    size_t;
#else
typedef uint32_t    size_t;
#endif

#ifdef __x86_64__
typedef uint64_t   phys_addr_t;
typedef uint64_t   virt_addr_t;
#else
typedef uint32_t   phys_addr_t;
typedef uint64_t   virt_addr_t;
#endif

typedef enum {false = 0, true = 1} bool;

#ifdef __x86_64__
typedef uint64_t   register_t;
#else
typedef uint64_t   register_t;
#endif

typedef uint64_t cpuid_t;
typedef uint32_t pid_t;

#endif  /* types.h */
