#include <const.h>
#include <types.h>
#include <page.h>
#include <panic.h>
#include <string.h>
#include <printk.h>
#include <config.h>

#define __1KB_SHIFT 10

struct block_node {
  phys_addr_t b_start_addr;
  phys_addr_t b_end_addr;
  int b_pages;
  bool b_avl;
  struct block_node *next;
  struct block_node *pre;
};

static struct block_node *block_node_array = NULL;
static struct block_node *blist = NULL;
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

  available_free_memory -= all_nodes * sizeof (struct block_node);
  first_free_addr    += all_nodes * sizeof (struct block_node);

  first_free_addr = ALIGN (first_free_addr, USER_VMS_PAGE_SIZE);

  for (i = 0; i < all_nodes; i++) {
    block_node_array[i].b_pages = 0;       /* This means its free */
  }
}

static struct __warn_unused_result block_node *
alloc_block_node (void)
{
  int i;

  for (i = 0; i < all_nodes; i++) {
    if (block_node_array[i].b_pages == 0) {
      return &block_node_array[i];
    }
  }
  return NULL;
}

static void
free_block_node (struct block_node *addr)
{
  int i;

  for (i = 0; i < all_nodes; i++) {
    if (&block_node_array[i] == addr) {
      block_node_array[i].b_pages = 0;
    }
  }
}

static void
init_block_list (void)
{
  size_t all_pages = available_free_memory / USER_VMS_PAGE_SIZE;
  blist = alloc_block_node ();
  if (blist == NULL) {
    cprintk ("Failed to initialize memory!\n", 0x4);
    __asm__ __volatile__ ("cli;hlt;\n\t");
  }

  blist->b_start_addr = first_free_addr;
  blist->b_end_addr   = last_free_addr;
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

      new_node = alloc_block_node ();

      new_node->b_start_addr = current_node->b_start_addr + n * USER_VMS_PAGE_SIZE;
      new_node->b_end_addr   = current_node->b_end_addr;
      new_node->b_pages      = current_node->b_pages - n;
      new_node->pre          = current_node;
      new_node->next         = current_node->next;
      new_node->b_avl        = true;

      current_node->b_pages = n;
      current_node->b_end_addr  = new_node->b_start_addr;
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
#if 0
void get_memory_status (void)
{
  struct block_node *current_node;

  current_node = blist;

  cprintk ("=============== MEMORY LAYOUT ===============\n", 0xB);
  while (current_node != NULL) {
    unsigned long int i= 0;
    for (i = 0; i < 9999900; i++);
    cprintk ("block address = %x pages = %x free = %d\n", 0xB, current_node->b_start_addr, current_node->b_pages, current_node->b_avl);
    current_node = current_node->next;
  }
}
#endif
void
free_mem_pages (phys_addr_t addr)
{
  struct block_node *current_node;

  current_node = blist;

  while (current_node != NULL) {
    if (current_node->b_start_addr == addr) {
      /*
       * Is block really allocated?
       */
      if (current_node->b_avl == false) {
        /*
         * In order to avoid defragmentation, we try to fusion free blocks
         * together.
         */
        if (current_node->pre != NULL && current_node->pre->b_avl == true) {
          current_node = current_node->pre;
          current_node->b_pages += current_node->next->b_pages;
          current_node->b_end_addr = current_node->next->b_end_addr;
          free_block_node (current_node->next);
          current_node->next = current_node->next->next;
          if (current_node->next) {
            current_node->next->pre = current_node;
          }
        }
        if (current_node->next != NULL && current_node->next->b_avl == true) {
          current_node->b_pages += current_node->next->b_pages;
          current_node->b_end_addr = current_node->next->b_end_addr;
          free_block_node (current_node->next);
          current_node->next = current_node->next->next;
          if (current_node->next) {
            current_node->next->pre = current_node;
          }
        }
        /* The block is now free */
        current_node->b_avl = true;
      } else {
        break;
      }
    }
    current_node = current_node->next;
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
