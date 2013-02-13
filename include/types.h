#ifndef __TYPES_H
#define __TYPES_H

#include <cdef.h>

#ifndef NULL
#define NULL  ((void*)0)
#endif

typedef int aint __aligned (0x10);

typedef unsigned char		        uint8_t;
typedef unsigned short int	    uint16_t;
typedef unsigned int		        uint32_t;
typedef uint8_t  auint8_t  __aligned (16);
typedef uint16_t auint16_t __aligned (16);
typedef uint32_t auint32_t __aligned (16);
#ifdef __x86_64__
typedef long int		  int64_t;
#else
typedef long long int int64_t;
#endif
typedef int64_t aint64_t __aligned (16);

typedef signed char		int8_t;
typedef short int		  int16_t;
typedef int			      int32_t;
typedef int8_t  aint8_t  __aligned (16);
typedef int16_t aint16_t __aligned (16);
typedef int32_t aint32_t __aligned (16);
#if __x86_64__
typedef unsigned long int	      uint64_t;
#else
typedef unsigned long long int	uint64_t;
#endif
typedef uint64_t auint64_t __aligned (16);

/* Types for `void *' pointers.  */
#ifdef __x86_64
typedef long int		      intptr_t;
typedef unsigned long int	uintptr_t;
#else
typedef int		         	intptr_t;
typedef unsigned int		uintptr_t;
#endif
typedef intptr_t  aintptr_t __aligned (16);
typedef uintptr_t auintptr_t __aligned (16);

#ifdef __x86_64__
typedef uint64_t    size_t;
#else
typedef uint32_t    size_t;
#endif
typedef size_t asize_t __aligned (16);

#ifdef __x86_64__
typedef uint64_t   phys_addr_t;
typedef uint64_t   virt_addr_t;
#else
typedef uint32_t   phys_addr_t;
typedef uint64_t   virt_addr_t;
#endif

typedef phys_addr_t aphys_addr_t __aligned (16);
typedef virt_addr_t avirt_addr_t __aligned (16);

typedef enum {false = 0, true = 1} bool;
typedef bool abool __aligned (16);

#ifdef __x86_64__
typedef uint64_t   register_t;
#else
typedef uint64_t   register_t;
#endif

typedef register_t aregister_t __aligned (16);

typedef uint64_t cpuid_t;
typedef cpuid_t acpuid_t __aligned (16);
typedef int32_t pid_t;
typedef pid_t apid_t __aligned (16);

#endif  /* types.h */
