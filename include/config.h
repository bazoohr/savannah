#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <const.h>

#define MIN_CPUS 4
#define MAX_CPUS 255

#define VMM_MAX_SIZE _2MB_
#define VM_MAX_SIZE  _2MB_

#define VMM_STACK_SIZE  0x1000
#define VM_STACK_SIZE   0x1000

#define NUMBER_SERVERS 3 /* Number of servers to run at booting time */
#define NUMBER_USER_VMS 1
#define ALWAYS_BUSY (NUMBER_SERVERS + NUMBER_USER_VMS)

#define NUMBER_MODULES 8

#define RS  0
#define PM	1
#define FS	2
#define INIT  3

#define KBD   4
#define CONSOLE 5

#define BSP_PAGE_SIZE  _2MB_
#define SRVR_VMS_PAGE_SIZE _2MB_
#define USER_VMS_PAGE_SIZE _4KB_

#define CHANNEL_SIZE USER_VMS_PAGE_SIZE

#define MAX_FD (32)

#define MAX_ARGV 16
#define MAX_ARGV_LEN 256
#endif /* __CONFIG_H__ */
