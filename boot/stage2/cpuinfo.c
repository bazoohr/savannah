#include <types.h>
#include <printk.h>
#include <cpu.h>
#include <page.h>
#include <mp.h>
#include <memory.h>
#include <panic.h>
/* ================================================== */
static struct cpu_info *cpuinfo_table[MAX_CPUS];
static int ncpus = 0;
/* ================================================== */
static struct cpu_info *cpuinfo_pool;
/* ================================================== */
uint64_t
get_cpu_freq (void)
{
  uint32_t tmp;
  uint64_t before;
  uint64_t after;

  //initialize PIT Ch 2 in one-shot mode
  //waiting 1 sec could slow down boot time considerably,
  //so we'll wait 1/100 sec, and multiply the counted ticks
  outb((inb (0x61) & 0x0C) | 1, 0x61);
  outb (0xB0, 0x43);
  //1193180/100 Hz  = 11931 = 0x2E9B
  //1193180/1000 Hz = 1193  = 0x04A9
  outb (0x9B, 0x42);  //LSB
  inb (0x60); //short delay
  outb (0x2E, 0x42);  //MSB

  //reset PIT one-shot counter (start counting)
  tmp = inb(0x61) & 0xFE;
  outb ((uint8_t)tmp, 0x61);    //gate low
  outb ((uint8_t)tmp | 1, 0x61);    //gate high

  //now wait until PIT counter reaches zero
  before = rdtsc ();
  while(! (inb (0x61) & 0x20));
  after = rdtsc ();

  return (after - before) * 100;
}
/* ================================================== */
static void
allocate_cpuinfo_pool (void)
{
  uint64_t max_pool_size = MAX_CPUS * _4KB_;

  if (sizeof (struct cpu_info) > _4KB_) {
    panic ("CPU info structure is too big. 4KB structure is accepted but the size is %d!\n", sizeof (struct cpu_info));
  }

  if (max_pool_size > _2MB_) {
    panic ("Second Boot Stage: Max CPU information Pool is bigger than 2MB\n");
  }

  cpuinfo_pool = (struct cpu_info *)calloc_align (sizeof (uint8_t), _2MB_, _2MB_);
  /* We put the first CPU in cpuinfo table */
  cpuinfo_table[0] = cpuinfo_pool;
}
/* ================================================== */
struct cpu_info *
add_cpu (void)
{
  static bool first_time = true;

  if (ncpus > MAX_CPUS) {
    panic ("Too many CPUs");
  }

  if (first_time) {
    first_time = false;
    allocate_cpuinfo_pool ();
  }

  cpuinfo_table[ncpus] = (struct cpu_info *)((phys_addr_t)cpuinfo_pool + (ncpus * _4KB_));
  return cpuinfo_table[ncpus++];
}
/* ================================================== */
int
get_ncpus (void)
{
  return ncpus;
}
/* ================================================== */
struct cpu_info *
get_cpu_info (cpuid_t cpuid)
{
  if (cpuid > MAX_CPUS) {
    panic ("get_cpu_info: No CPU with id %d\n", cpuid);
  }
  return cpuinfo_table[cpuid];
}
