#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <const.h>
#include <macro.h>

#define VMM_MAX_SIZE _2MB_
#define VM_MAX_SIZE  _2MB_

#define VMM_STACK_SIZE  0x1000
#define VM_STACK_SIZE   0x1000

#define NUMBER_SERVERS 2 /* Number of servers to run at booting time */

#define BSP_PAGE_SIZE  _2MB_
#define SRVR_VMS_PAGE_SIZE _2MB_
#define USER_VMS_PAGE_SIZE _4KB_

#endif /* __CONFIG_H__ */
