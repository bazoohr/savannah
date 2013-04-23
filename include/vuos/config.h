#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <const.h>

#define MIN_CPUS 4
#define MAX_CPUS 255

#define VMM_MAX_SIZE _2MB_
#define VM_MAX_SIZE  _2MB_

#define VMM_STACK_SIZE  0x1000
#define VM_STACK_SIZE   0x1000

#define NUMBER_SERVERS 2 /* Number of servers to run at booting time */
#define NUMBER_USER_VMS 1
#define ALWAYS_BUSY (NUMBER_SERVERS + NUMBER_USER_VMS)

#define NUMBER_MODULES 7

#define PM	0
#define FS	1
#define INIT  2

#define KBD   3
#define JUNK  4
#define CONSOLE 5  /* Console MUST be the last one */

/* IRQs */
#define TMR_IRQ 32
#define KBD_IRQ 33
#define IPI_IRQ 34
#define NET_IRQ 35
/* ================================== */
#define BSP_PAGE_SIZE  _2MB_
#define SRVR_VMS_PAGE_SIZE _2MB_
#define USER_VMS_PAGE_SIZE _4KB_

#define CHANNEL_SIZE USER_VMS_PAGE_SIZE

#define MAX_FD (32)
#define MAX_CHANNELS (32)

#define MAX_ARGV 16
#define MAX_ARGV_LEN 256
#endif /* __CONFIG_H__ */
