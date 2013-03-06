#ifndef __PM_ARGS_H
#define __PM_ARGS_H

struct pm_args {
  size_t memory_size;
  size_t has_1GB_page;
  phys_addr_t last_used_addr;
};

#endif /* __PM_ARGS_H */
