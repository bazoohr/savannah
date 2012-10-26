#ifndef __CDEF_H__
#define __CDEF_H__

#include <types.h>
/* =========================================== */
#if __GNUC__

#ifndef __offsetof
#define __offsetof(type, field)	 __builtin_offsetof(type, field)
#endif

#else   /* __GNUC__ */

#define	__offsetof(type, field)	((size_t)(&((type *)0)->field))

#endif   /* __GNUC__ */
/* =========================================== */
#ifndef offsetof
# define offsetof(type, field) __offsetof(type, field)
#endif
/* =========================================== */
#define __packed  __attribute__((packed))
/* =========================================== */
#define __aligned(x) __attribute__((aligned(x)))
/* =========================================== */
#define __noreturn  __attribute__((noreturn))
/* =========================================== */
#define __unused __attribute__((unused))
/* =========================================== */
#define __warn_unused_result __attribute__((warn_unused_result))
/* =========================================== */
#ifndef __always_inline
#define __always_inline /*__attribute__((always_inline))*/
#endif
/* =========================================== */
#ifndef __inline
#define	__inline	__always_inline
#endif

#endif   /* __CDEF_H__   */

