#include <const.h>
#include <types.h>
#include <page.h>
#include <panic.h>
#include <string.h>
#include <printk.h>
#include <config.h>
#include <macro.h>
#include <printk.h>

struct block_node {
  phys_addr_t b_start_addr;
  int b_pages;
  bool b_avl;
  struct block_node *next;
  struct block_node *pre;
};

static struct block_node *block_node_array = NULL;
static struct block_node *blist = NULL;
static int block_node_array_last_idx;
static size_t all_nodes;

static phys_addr_t first_free_addr;
static phys_addr_t last_free_addr;
static size_t total_memory;
static size_t available_free_memory;

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
  all_nodes = available_free_memory / USER_VMS_PAGE_SIZE;

  block_node_array = (struct block_node *)first_free_addr;

  if (all_nodes * sizeof (struct block_node) > available_free_memory) {
    panic ("memory manager fatal (%s): Not enough memory!\n", __func__);
  }

  block_node_array_last_idx = all_nodes;

  available_free_memory -= all_nodes * sizeof (struct block_node);
  first_free_addr    += all_nodes * sizeof (struct block_node);

  first_free_addr = ALIGN (first_free_addr, USER_VMS_PAGE_SIZE);

  for (i = 0; i < all_nodes; i++) {
    block_node_array[i].b_avl = true;
    block_node_array[i].b_pages = 0;       /* This means its free */
  }
}

static struct __warn_unused_result block_node *
alloc_block_node (phys_addr_t paddr)
{
  int idx;
  struct block_node *block;
  /* ---------------------------- */
  if (paddr & 0xFFF) {
    panic ("memory manager fatal (%s): memory address %x is not 4KB aligned!\n", __func__, paddr);
  }
  /* ---------------------------- */
  if (paddr < first_free_addr) {
    panic ("memory manager fatal (%s): physical address %x under range!", __func__, paddr);
  }
  /* ---------------------------- */
  idx = (paddr - first_free_addr) / USER_VMS_PAGE_SIZE;
  if (idx > block_node_array_last_idx) {
    panic ("memory manager fatal (%s): block index %d is over block array size", __func__, idx);
  }
  /* ---------------------------- */
  block = &block_node_array[idx];
  if (block->b_avl == false) {
    panic ("memory manager fatal (%s): block %x is already allocated!", __func__, block);
  }
  /* ---------------------------- */
  return block;
}

static void
init_block_list (void)
{
  size_t all_pages = available_free_memory / USER_VMS_PAGE_SIZE;
  blist = alloc_block_node (first_free_addr);
  if (blist == NULL) {
    panic ("memory manager fatal (%s): Failed to initialize memory!\n", __func__);
  }

  blist->b_start_addr = first_free_addr;
  blist->b_pages      = all_pages;
  blist->b_avl        = true;

  blist->next = NULL;
  blist->pre  = NULL;
}

phys_addr_t __warn_unused_result
alloc_mem_pages (size_t n)
{
  struct block_node *current_node;
  phys_addr_t found_block = 0;

  current_node = blist;

  while (current_node != NULL) {
    if (current_node->b_avl == false || current_node->b_pages < n) {
      current_node = current_node->next;
    } else if (current_node->b_pages == n) {
        current_node->b_avl = false;
        found_block = current_node->b_start_addr;
        break;
    } else {
      struct block_node *new_node;
      phys_addr_t new_node_paddr;

      new_node_paddr = current_node->b_start_addr + n * USER_VMS_PAGE_SIZE;
      new_node = alloc_block_node (new_node_paddr);

      new_node->b_start_addr = new_node_paddr;
      new_node->b_pages      = current_node->b_pages - n;
      new_node->pre          = current_node;
      new_node->next         = current_node->next;
      new_node->b_avl        = true;

      current_node->b_pages = n;
      current_node->b_avl = false;
      current_node->next = new_node;

     found_block = current_node->b_start_addr;
     break;
    }
  } /* While */


  return found_block;
}

phys_addr_t __warn_unused_result
alloc_clean_mem_pages (size_t n)
{
  phys_addr_t found_block;

  found_block = alloc_mem_pages (n);

  if (found_block > 0) {
    memset ((void *)found_block, 0, n * USER_VMS_PAGE_SIZE);
  }

  return found_block;
}

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
void
free_mem_pages (phys_addr_t paddr)
{
  int idx;
  struct block_node *block;
  /* ---------------------------- */
  if (paddr & 0xFFF) {
    panic ("memory manager fatal (%s): memory address %x is not 4KB aligned!\n", __func__, paddr);
  }
  /* ---------------------------- */
  if (paddr < first_free_addr) {
    panic ("memory manager fatal (%s): physical address %x under range!", __func__, paddr);
  }
  /* ---------------------------- */
  idx = (paddr - first_free_addr) / _4KB_;
  if (idx > block_node_array_last_idx) {
    panic ("memory manager fatal (%s): block index %d is over block array size", __func__, idx);
  }
  /* ---------------------------- */
  block = &block_node_array[idx];
  if (block->b_avl) {
    panic ("memory manager fatal (%s): Can't free a free block!", __func__);
  }

  if (!block->b_avl) {
    /*
     * In order to avoid defragmentation, we try to fusion free blocks
     * together.
     */
    if (block->pre && block->pre->b_avl) {
      block = block->pre;
      block->b_pages += block->next->b_pages;
      block->next->b_avl = true;
      //free_block_node (block->next);
      block->next = block->next->next;
      if (block->next) {
        block->next->pre = block;
      }
    }
    if (block->next && block->next->b_avl) {
      block->b_pages += block->next->b_pages;
      block->next->b_avl = true;
      //free_block_node (block->next);
      block->next = block->next->next;
      if (block->next) {
        block->next->pre = block;
      }
    }
    /* The block is now free */
    block->b_avl = true;
  } else {
    panic ("memory manager fata (%s): Can't free a free block address %x!\n", __func__, paddr);
  }
}

void
memory_init (const phys_addr_t last_allocated_byte, const size_t mem_size)
{
  total_memory       = mem_size;
  first_free_addr    = last_allocated_byte;
  last_free_addr     = total_memory - (total_memory % USER_VMS_PAGE_SIZE);
  available_free_memory = last_free_addr - first_free_addr;

  init_block_node ();
  init_block_list ();
}
