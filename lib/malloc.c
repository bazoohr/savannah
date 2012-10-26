/* ================================================ *
 * malloc.c                                         *
 *                                                  *
 * Hamid R. Bazoobandi 2012                         *
 * Amsterdam, The Netherlands                       *
 * ================================================ */
#include <cdef.h>
#include <macro.h>
#include <types.h>
#include <asmlib.h>
#include <io.h>
/* ================================================ */
#define METADATA_SIZE   (offsetof (struct m_block, mb_data) - offsetof (struct m_block, mb_size))
/* ================================================ */
struct m_block {
  size_t mb_size;          /* Size of memory block   */
  struct m_block *mb_nxt;  /* next memory block      */
  struct m_block *mb_pre;  /* previous memory block  */
  bool   mb_free;          /* Is this block free?    */
  uint8_t *mb_this;        /* points to mb_data[1]   */
  uint8_t mb_data[1];      /* Starting byte of block */
};
/* ================================================ */
static struct m_block *head = NULL;
static struct m_block *last = NULL;
/* ================================================ */
static struct m_block * __warn_unused_result
extend_heap (size_t size)
{
  struct m_block *new_mblock;

  new_mblock = sbrk (METADATA_SIZE + size);
  if (new_mblock == (void *)-1) {
    return NULL;
  }

  new_mblock->mb_size = size;
  new_mblock->mb_nxt  = NULL;
  new_mblock->mb_pre  = last;
  new_mblock->mb_this = new_mblock->mb_data;
  new_mblock->mb_free = false;
  
  if (!head) {
    head = new_mblock;
  }

  if (last) {
    last->mb_nxt = new_mblock;
  }
  last = new_mblock;

  return new_mblock;
}
/* ================================================ */
static struct m_block * __warn_unused_result
find_block (size_t size)
{
  struct m_block *current = head;

  while (current && !(current->mb_free && current->mb_size >= size)) {
    current = current->mb_nxt;
  }

  return current;
}
/* ================================================ */
static void
split_block (struct m_block *block, size_t size)
{
  struct m_block *new_mblock;

  new_mblock = (struct m_block *)(block->mb_data + size);
  
  new_mblock->mb_size = block->mb_size - (METADATA_SIZE + size);
  new_mblock->mb_nxt  = block->mb_nxt;
  new_mblock->mb_pre  = new_mblock;
  new_mblock->mb_this = new_mblock->mb_data;
  new_mblock->mb_free = true;

  block->mb_size = size;
  block->mb_nxt  = new_mblock;

  if (new_mblock->mb_nxt) {
    new_mblock->mb_nxt->mb_pre = new_mblock;
  }

}
/* ================================================ */
static struct m_block * __warn_unused_result
get_mblock (void *ptr)
{
  uint8_t *data_block = (uint8_t*)ptr;
  return (struct m_block*)(data_block - METADATA_SIZE);
}
/* ================================================ */
static bool __warn_unused_result
is_valid_addr (void *addr)
{
  if (head) {
    if (addr > (void*)head && addr < sbrk (0)) {
      return addr == get_mblock (addr)->mb_this;
    }
  }
  return false;
}
/* ================================================ */
static struct m_block *
fusion (struct m_block *block)
{
  if (block->mb_nxt && block->mb_nxt->mb_free) {
    block->mb_size += METADATA_SIZE + block->mb_nxt->mb_size;
    block->mb_nxt = block->mb_nxt->mb_nxt;

    if (block->mb_nxt) {
      block->mb_nxt->mb_pre = block;
    }
  }
  return block;
}
/* ================================================ */
void * __warn_unused_result
malloc (size_t size)
{
  struct m_block *block;
  size_t needed_bytes;

  needed_bytes = ALIGN (size, 4);

  if (head) {
    block = find_block (needed_bytes);

    if (block) {
      if ((block->mb_size - size) >= (METADATA_SIZE + 4)) {
        split_block (block, size);
      }
      block->mb_free = false;
    } else {
      block = extend_heap (size);
      if (!block) {
        return NULL;
      }
    }
  } else {
    block = extend_heap (size);
    if (!block) {
      return NULL;
    }
  }
  return block->mb_data;
}
/* ================================================ */
void
free (void *ptr)
{
  if (is_valid_addr (ptr)) {
    struct m_block *block;

    block = get_mblock (ptr);
    block->mb_free = 1;
    
    /* Fusion with previous one if possible */
    if (block->mb_pre && block->mb_pre->mb_free) {
      block = fusion (block->mb_pre);
    }
    /* Fusion with next one if possible */
    if (block->mb_nxt) {
      fusion (block);
    } else {
      /* Free the end of the heap */
      if (!block->mb_pre) {
        /* No more blocks */
        head = NULL;
        last = NULL;
        if (brk (block) < 0) {
          cprintf ("free: brk() failed!\n", 0x4);
        }
      } else {
        last = block->mb_pre;
        last->mb_nxt = NULL;
      }
    }
  } else {
    cprintf ("free: Invalid Address %x\n", 0x4, ptr);
  }
}
