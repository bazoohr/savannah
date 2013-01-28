#ifndef __ASMLINK_H__
#define __ASMLINK_H__

#define ALIGN_DATA	.p2align 3	/* 8 byte alignment, zero filled */
#ifdef GPROF
#define ALIGN_TEXT	.p2align 4,0x90	/* 16-byte alignment, nop filled */
#else
#define ALIGN_TEXT	.p2align 4,0x90	/* 16-byte alignment, nop filled */
#endif

/*
 * Convenience macro for declaring interrupt entry points.
 */
#define	TRAPENT(name)	ALIGN_TEXT; .globl name;\
			.type name, @function; name:

#define ISRENT(name) ALIGN_TEXT; .globl name;\
      .type name, @function; name:
/*
 * For general assembly routines we're using these macros
 */
#define ENTRY(name)       \
  ALIGN_TEXT;             \
  .globl name;            \
  .type name, @function;  \
name:                    

#define END(name)     \
  .size name, .-name

#endif /* __ASMLINK_H__ */
