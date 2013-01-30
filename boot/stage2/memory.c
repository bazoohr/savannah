#include <types.h>
#include <page.h>
/* ============================================== */
#define ALIGN(addr, bound) (((addr)+((bound)-1))&(~((bound)-1)))
/* ============================================== */
static size_t memory_size;
static phys_addr_t first_free_addr
static phys_addr_t boot_stage2_heap_start_addr;
/* ============================================== */
void *
malloc (size_t size)
{
  if (size == 0) {
    return NULL;
  }

  if (memory_size == 0 && 
      first_free_addr == 0 &&
      boot_stage2_heap_start_addr == 0) {
    panic ("Boot Stage2: Memory is not initialized!\n");
  }

  if (first_free_addr + size > memory_size) {
    return NULL;
  }

  return (first_free_addr += size);
}

/* ============================================== */
void *
calloc (size_t nmemb, size_t size)
{
  if (size == 0) {
    return NULL;
  }

  if (memory_size == 0 && 
      first_free_addr == 0 &&
      boot_stage2_heap_start_addr == 0) {
    panic ("Boot Stage2: Memory is not initialized!\n");
  }

  if (first_free_addr + size > memory_size) {
    return NULL;
  }

  return (first_free_addr += size);
}
/* ============================================== */
void *
malloc_align (size_t size, size_t alignment)
{
  if (size == 0) {
    return NULL;
  }

  if (memory_size == 0 && 
      first_free_addr == 0 &&
      boot_stage2_heap_start_addr == 0) {
    panic ("Boot Stage2: Memory is not initialized!\n");
  }

  if (alignment >= memory_size) {
    return NULL;
  }

  first_free_addr = ALIGN (first_free_addr, alignment);
  if (first_free_addr + size > memory_size) {
    return NULL;
  }

  return (first_free_addr += size);
}
/* ============================================== */
void
init_memory (phys_addr_t last_allocated_byte, size_t mem_size)
{
  mem_size = mem_size;
  first_free_addr = last_allocated_byte;
  boot_stage_2_heap_start_addr = last_allocated_byte;
}
