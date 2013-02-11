#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <const.h>
#include <macro.h>

#define VMM_MAX_SIZE _2MB_
#define VM_MAX_SIZE  _2MB_

#define VMM_STACK_SIZE  0x1000
#define VM_STACK_SIZE   0x1000

#define NUMBER_SERVERS 2 /* Number of servers to run at booting time */
#define NUMBER_USER_VMS 1
#define NUMBER_MODULES 7

#define PM	0
#define FS	1
#define INIT    2

#define BSP_PAGE_SIZE  _2MB_
#define SRVR_VMS_PAGE_SIZE _2MB_
#define USER_VMS_PAGE_SIZE _4KB_

#define MAX_ARGV 16
#define MAX_ARGV_LEN 256
#endif /* __CONFIG_H__ */
