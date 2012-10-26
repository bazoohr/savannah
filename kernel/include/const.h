#ifndef __CONST_H__
#define __CONST_H__

#define BOOT_CODE_SEL  0x10  /* Boot code selector */

#define KNL_CSEL       0x10  /* Kernel code selector in GDT */
#define KNL_DSEL       0x20  /* Kernel data selector in GDT */
#define TSS_SEL        0x30  /* TSS selector in GDT */
#define LDT_SEL        0x40  /* LDT selector in GDT */

#define NPID 65535  /* Max number of processes */

#define NGDT  5   /* FIXME: This is just an arbitrary value. Compute the right
                    * value according to kernel needs. My Guess is that 5 should be OK
                    * 1. Empty, 2. code, 3. data, 4. TSS, 5. ldt
                    */
#define NIDT 256    /* Number of entries in IDT */

#define GDT_KCODE      1     /* Code descriptor index in GDT */
#define GDT_KDATA      2     /* Data descriptor index in GDT */
#define GDT_TSS        3     /* TSS descriptor index in GDT  */
#define GDT_LDT        4     /* LDT descriptor index in GDT  */

#define KNL_PVL        0     /* Kernel privilege level */
#define USR_PVL        3     /* User privilege level */

#define USR_CSEL       0x7   /* User processes code selector */
#define USR_DSEL       0xF   /* User processes data selector */
#define SYS_CSEL       0x4   /* System processes code selector */
#define SYS_DSEL       0xC   /* System processes data selector */
#define TRAP_SYSCALL   0x80  /* Trap to kernel */

#endif /* __CONST_H__ */
