#include <types.h>
#include <page.h>
#include <panic.h>
#include <string.h>
#include <printk.h>
/* ============================================== */
#define ALIGN(addr, bound) (((addr)+((bound)-1))&(~((bound)-1)))
/* ============================================== */
static size_t memory_size;
static phys_addr_t first_free_addr;
/* ============================================== */
void *
malloc (size_t size)
{
  phys_addr_t block_start_addr;

  if (size == 0) {
    return NULL;
  }

  if (memory_size == 0 && first_free_addr == 0) {
    panic ("Boot Stage2: Memory is not initialized!\n");
  }

  if (first_free_addr + size > memory_size) {
    cprintk ("first_free_addr = %x size = %x memory_size - %x\n", 0xa, first_free_addr, size, memory_size);
    cprintk ("first_free_addr + size = %x memory_size = %x\n", 0xa, first_free_addr + size, memory_size);
    return NULL;
  }

  block_start_addr = first_free_addr;
  first_free_addr += size;

  first_free_addr = ALIGN (first_free_addr, 16);

  return (void *)block_start_addr;
}
/* ============================================== */
void *
malloc_align (size_t size, size_t alignment)
{
  phys_addr_t block_start_addr;

  if (size == 0) {
    return NULL;
  }

  if (memory_size == 0 && first_free_addr == 0) {
    panic ("Boot Stage2: Memory is not initialized!\n");
  }

  if (alignment >= memory_size) {
    return NULL;
  }

  first_free_addr = ALIGN (first_free_addr, alignment);
  if (first_free_addr + size > memory_size) {
    return NULL;
  }

  block_start_addr = first_free_addr;
  first_free_addr += size;

  first_free_addr = ALIGN (first_free_addr, 16);

  return (void *)block_start_addr;
}
/* ============================================== */
void *
calloc (size_t nmemb, size_t size)
{
  size_t total_size;
  phys_addr_t block_start_addr;

  if (size == 0 || nmemb == 0) {
    return NULL;
  }

  if (memory_size == 0 && first_free_addr == 0) {
    panic ("Boot Stage2: Memory is not initialized!\n");
  }

  total_size = nmemb * size;

  if (first_free_addr + total_size > memory_size) {
    return NULL;
  }

  block_start_addr = first_free_addr;
  memset ((void *)block_start_addr, 0, total_size);

  first_free_addr += total_size;
  first_free_addr = ALIGN (first_free_addr, 16);

  return (void *)block_start_addr;
}

/* ============================================== */
void *
calloc_align (size_t nmemb, size_t size, size_t alignment)
{
  size_t total_size;
  phys_addr_t block_start_addr;

  if (size == 0 || nmemb == 0) {
    return NULL;
  }

  if (memory_size == 0 && first_free_addr == 0) {
    panic ("Boot Stage2: Memory is not initialized!\n");
  }

  if (alignment >= memory_size) {
    return NULL;
  }

  total_size = nmemb * size;

  first_free_addr = ALIGN (first_free_addr, alignment);
  if (first_free_addr + total_size > memory_size) {
    return NULL;
  }

  block_start_addr = first_free_addr;
  memset ((void *)block_start_addr, 0, total_size);

  first_free_addr += total_size;
  first_free_addr = ALIGN (first_free_addr, 16);

  return (void *)block_start_addr;
}
/* ============================================== */
void
memory_init (phys_addr_t last_allocated_byte, size_t mem_size)
{
  memory_size = mem_size * _1MB_;
  first_free_addr = ALIGN (last_allocated_byte, 16);
}
