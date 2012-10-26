#include <types.h>
#include <proc.h>
#include <printk.h>
#include <cdef.h>
#include <pid.h>
/* ========================================= */
#define FREE 0
#define BUSY 1
/* ========================================= */
static struct proc proc_table[NPID] __aligned(16);
static pid_t pid_map[NPID] __aligned(16);
/* ========================================= */
/* FIXME:
 * Just an optimization. Since init and idel processes have
 * a pre-known PID, and they are always there, we can set the 
 * lower bound of for loops in this file accordilngly and save a few
 * cycles.
 */
/* ========================================= */
static void
init_pid ()
{
  register int i;
	for (i = 0; i < NPID; i++)
		pid_map[i] = FREE;
}
/* ========================================= */
pid_t
alloc_pid (void)
{
	register int i;
	for (i = 0; i < NPID; i++){
		if (pid_map[i] == FREE){
			pid_map[i] = BUSY;
			return i;
		}
	}
	printk ("Max Proc was allocated\n\t");
	for (;;);
}
/* ========================================= */
void
free_pid (pid_t pid)
{
  if (pid >= NPID) {
    cprintk ("ERROR: PID too big!\n", 0x4);
    __asm__ __volatile__ ("cli;hlt\n\t");
  }
	pid_map[pid] = FREE;
}
/* ========================================= */
void
do_getpid (void)
{
}
/* ========================================= */
void
do_getppid (void)
{
}
/* ========================================= */
struct proc*
get_pcb (pid_t pid)
{
  if (pid >= NPID) {
    cprintk ("ERROR: PID too big!\n", 0x4);
    __asm__ __volatile__ ("cli;hlt\n\t");
  }
  return &proc_table[pid];
}
/* ========================================= */
struct proc*
alloc_pcb (pid_t pid)
{
  if (pid >= NPID) {
    cprintk ("ERROR: PID too big!\n", 0x4);
    __asm__ __volatile__ ("cli;hlt\n\t");
  }
	return &proc_table[pid];
}
/* ========================================= */
void
pm_init (void)
{
	init_pid ();
}
