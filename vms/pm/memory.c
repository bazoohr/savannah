#include <const.h>
#include <types.h>
#include <page.h>
#include <panic.h>
#include <string.h>
#include <vuos/vuos.h>
#include <macro.h>
#include <debug.h>
#include <asmfunc.h>
/* =================================================== */
#if USER_VMS_PAGE_SIZE > _4KB_
#warning "This configuration may break memory manager!"
#endif
/* =================================================== */
#define MIN_MEM_BLOCK_SIZE USER_VMS_PAGE_SIZE
/* =================================================== */
struct block_node {
  phys_addr_t b_start_addr;
  int b_pages;
  uint32_t b_ref;
  struct block_node *next;
  struct block_node *pre;
};
/* =================================================== */
static struct block_node *block_node_array = NULL;
static struct block_node *blist = NULL;
static int block_node_array_last_idx;

static phys_addr_t first_free_addr;

static size_t all_nodes;
static size_t available_memory;
static size_t available_free_memory;
/* =================================================== */
static struct __warn_unused_result block_node *
alloc_block_node (phys_addr_t paddr)
{
  int idx;
  struct block_node *block;
  /* ---------------------------- */
  if (unlikely (paddr & 0xFFF)) {
    panic ("memory manager fatal (%s:%d): memory address %x is not 4KB aligned!\n", __func__, __LINE__, paddr);
  }
  /* ---------------------------- */
  if (unlikely (paddr < first_free_addr)) {
    panic ("memory manager fatal (%s:%d): physical address %x under range!", __func__, __LINE__, paddr);
  }
  /* ---------------------------- */
  idx = (paddr - first_free_addr) / MIN_MEM_BLOCK_SIZE;
  if (unlikely (idx > block_node_array_last_idx)) {
    panic ("memory manager fatal (%s:%d): block index %d is over block array size", __func__, __LINE__, idx);
  }
  /* ---------------------------- */
  block = &block_node_array[idx];
  if (unlikely (block->b_ref > 0)) {
    panic ("memory manager fatal (%s:%d): block %x is already allocated!", __func__, __LINE__, block);
  }
  return block;
}
/* =================================================== */
static void
init_block_node (void)
{
  int i;
  /*
   * Well, I know this way we are wasting a bit of memory, because
   * we are reserving memory for more nodes than we actually need; But
   * at least we're preserving the simplicity.
   * There should be very better ways of finding out acurately how many
   * nodes we have to allocate. But I'm ganna do this in the simplest form.
   */
  all_nodes = available_free_memory / MIN_MEM_BLOCK_SIZE + (available_free_memory % MIN_MEM_BLOCK_SIZE ? 1 : 0);

  block_node_array = (struct block_node *)first_free_addr;

  if (unlikely (all_nodes * sizeof (struct block_node) + first_free_addr > available_memory)) {
    panic ("memory manager fatal (%s:%d): Not enough memory!\n", __func__, __LINE__);
  }

  block_node_array_last_idx = all_nodes;

  first_free_addr += all_nodes * sizeof (struct block_node);
  first_free_addr = ALIGN (first_free_addr, MIN_MEM_BLOCK_SIZE);

  available_free_memory = available_memory - first_free_addr;

  for (i = 0; i < block_node_array_last_idx; i++) {
    block_node_array[i].b_ref = 0;
    block_node_array[i].b_pages = 0;
  }
}
/* =================================================== */
static void
init_block_list (void)
{
  size_t all_free_pages;

  all_free_pages = available_free_memory / MIN_MEM_BLOCK_SIZE;
  blist = alloc_block_node (first_free_addr);

  if (unlikely (blist == NULL)) {
    panic ("memory manager fatal (%s:%d): Failed to initialize memory!\n", __func__, __LINE__);
  }

  blist->b_start_addr = first_free_addr;
  blist->b_pages      = all_free_pages;
  blist->b_ref        = 0;

  blist->next = NULL;
  blist->pre  = NULL;
}
/* =================================================== */
phys_addr_t __warn_unused_result
alloc_mem_pages (size_t n)
{
  struct block_node *current_node;
  phys_addr_t found_block = 0;

  /*
   * Requesting for zero pages does not
   * make any sense :)
   */
  if (unlikely (n == 0)) {
    return (phys_addr_t)0;
  }

  current_node = blist;

  while (current_node != NULL) {
    if (current_node->b_ref > 0 || current_node->b_pages < n) {
      current_node = current_node->next;
    } else if (current_node->b_pages == n) {
      current_node->b_ref = 1;  /* first reference to this block */
      found_block = current_node->b_start_addr;
      break;
    } else {
      struct block_node *new_node;
      phys_addr_t new_node_paddr;

      new_node_paddr = current_node->b_start_addr + n * MIN_MEM_BLOCK_SIZE;
      new_node = alloc_block_node (new_node_paddr);

      new_node->b_start_addr = new_node_paddr;
      new_node->b_pages      = current_node->b_pages - n;
      new_node->pre          = current_node;
      new_node->next         = current_node->next;
      if (current_node->next) current_node->next->pre = new_node;
      new_node->b_ref        = 0;

      current_node->b_pages = n;
      current_node->b_ref = 1;    /* First reference to this block */
      current_node->next = new_node;

      found_block = current_node->b_start_addr;

      break;
    }
  } /* While */

  return found_block;
}
/* =================================================== */
bool __warn_unused_result
reallocable (phys_addr_t paddr)
{
  int idx;

  idx = (paddr - first_free_addr) / MIN_MEM_BLOCK_SIZE;

  return paddr > first_free_addr &&
         idx < block_node_array_last_idx ? true : false;
}
/* =================================================== */
phys_addr_t __warn_unused_result
realloc_mem_block (phys_addr_t paddr)
{
  int idx;
  struct block_node *block;
  /* ---------------------------- */
  if (unlikely (paddr & 0xFFF)) {
    panic ("Memory manager fatal (%s:%d): memory address %x is not 4KB aligned!\n", __func__, __LINE__, paddr);
  }
  /* ---------------------------- */
  if (unlikely (paddr < first_free_addr)) {
    panic ("Memory manager fatal (%s:%d): physical address %x under range!", __func__, __LINE__, paddr);
  }
  /* ---------------------------- */
  idx = (paddr - first_free_addr) / _4KB_;
  if (unlikely (idx > block_node_array_last_idx)) {
    panic ("Memory manager fatal (%s:%d): block index %d is over block array size", __func__, __LINE__, idx);
  }
  /* ---------------------------- */
  block = &block_node_array[idx];
  if (unlikely (block->b_ref == 0)) {
    panic ("Memory manager fatal (%s:%d): Can't reallocate a free memory block!", __func__, __LINE__);
  }
  /* ---------------------------- */
  block->b_ref++;
  return paddr;
}
/* =================================================== */
phys_addr_t __warn_unused_result
alloc_clean_mem_pages (size_t n)
{
  phys_addr_t found_block;

  found_block = alloc_mem_pages (n);

  if (likely (found_block > 0)) {
    memset ((void *)found_block, 0, n * MIN_MEM_BLOCK_SIZE);
  }

  return found_block;
}
/* =================================================== */
phys_addr_t __warn_unused_result
alloc_page_table (void)
{
  phys_addr_t found_block;

  found_block = alloc_mem_pages (1);

  if (found_block > 0) {
    memset ((void *)found_block, 0, PAGE_TABLE_SIZE);
  }

  return found_block;
}
/* =================================================== */
void
free_mem_pages (phys_addr_t paddr)
{
  int idx;
  struct block_node *block;
  /* ---------------------------- */
  if (unlikely (paddr & 0xFFF)) {
    panic ("Memory manager fatal (%s:%d): memory address %x is not 4KB aligned!\n", __func__, __LINE__, paddr);
  }
  /* ---------------------------- */
  if (unlikely (paddr < first_free_addr)) {
    panic ("Memory manager fatal (%s:%d): physical address %x under range!", __func__, __LINE__, paddr);
  }
  /* ---------------------------- */
  idx = (paddr - first_free_addr) / _4KB_;
  if (unlikely (idx > block_node_array_last_idx)) {
    panic ("Memory manager fatal (%s:%d): block index %d is over block array size", __func__, __LINE__, idx);
  }
  /* ---------------------------- */
  block = &block_node_array[idx];
  if (unlikely (block->b_ref == 0)) {
    panic ("Memory manager fatal (%s:%d): Can't free a free memory block!", __func__, __LINE__);
  }
  /* If this block is allocated more than once */
  if (block->b_ref > 1) {
    block->b_ref--;
    return;
  }

  /*
   * In order to avoid defragmentation, we try to fusion free blocks
   * together.
   */
  if (block->next && block->next->b_ref == 0) {
    struct block_node *next_node;

    next_node = block->next;

    if (unlikely (next_node == blist)) {
      panic ("Memory manager fatal (%s:%d): Memory block list is corrupted!!", __func__, __LINE__);
    }
    if (unlikely (next_node->pre != block)) {
      panic ("Memory Manager fatal (%s:%d): Memory block list is corrupted!!", __func__, __LINE__);
    }

    block->b_pages += next_node->b_pages;
    block->next = next_node->next;
    if (next_node->next) next_node->next->pre = block;
    next_node->b_ref = 0;
  }

  if (block->pre && block->pre->b_ref == 0) {
    struct block_node *previous_node;

    previous_node = block->pre;

    if (unlikely (previous_node->next != block)) {
      panic ("Memory Manager fatal (%s:%d): Memory block list is corrupted!!", __func__, __LINE__);
    }

    previous_node->b_pages += block->b_pages;
    previous_node->next = block->next;
    if (block->next) block->next->pre = previous_node;
    previous_node->b_ref = 0;
  }
  /* The block is now free */
  block->b_ref = 0;
}
/* =================================================== */
void
memory_init (const phys_addr_t last_allocated_byte, const size_t mem_size)
{
  available_memory = mem_size;
  first_free_addr  = last_allocated_byte;

  if (available_memory < first_free_addr) {
    panic ("Memory Manager fatal (%s:%d): Misconfigured memory parameters!", __func__, __LINE__);
  }

  available_free_memory = available_memory - first_free_addr;

  init_block_node ();
  init_block_list ();
}
