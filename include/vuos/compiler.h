#ifndef __COMPILER_H__
#define __COMPILER_H__


#ifdef __GNUC__

//# define likely(x)	__builtin_expect(!!(x), 1)
//# define unlikely(x)	__builtin_expect(!!(x), 0)

# define likely(x)	(x)
# define unlikely(x)	(x)
#else

#error "You should compile this code with GNU compatible compilers"
#define likely(x) (x)
#define unlikely(x) (x)

#endif

#endif /* __COMPILER_H__ */
