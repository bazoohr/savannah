#include <asmlib.h>
#include <proc.h>
#include <malloc.h>
#include <io.h>
#include <macro.h>
/* ==================================== */
#define MAX_THREADS 256
/* ==================================== */
struct thread_info {
  void *(*ti_routine)(void *args);
  void *ti_args;
  uint8_t *ti_stack;
  bool ti_avl;
  tid_t ti_tid;
};
/* ==================================== */
struct thread_info thread_info_table[MAX_THREADS];
static bool first_time = true;
/* ==================================== */
static void
thread_init (void)
{
  int i;
  for (i = 0; i < MAX_THREADS; i++) {
    thread_info_table[i].ti_avl = true;
  }
}
/* ==================================== */
static struct thread_info * __warn_unused_result
thread_get_info (tid_t tid)
{
  int i;
  for (i = 0; i < MAX_THREADS; i++) {
    if (thread_info_table[i].ti_tid == tid) {
      return &thread_info_table[i];
    }
  }

  return NULL;
}
/* ==================================== */
static struct thread_info * __warn_unused_result
thread_create_empty (void)
{
  int i;
  for (i = 0; i < MAX_THREADS; i++) {
    if (thread_info_table[i].ti_avl == true) {
      thread_info_table[i].ti_avl = false;
      return &thread_info_table[i];
    }
  }

  return NULL;
}
/* ==================================== */
static __inline void
thread_destroy (struct thread_info *t_info)
{
  free (t_info->ti_stack);
  t_info->ti_avl = true;
}
/* ==================================== */
static void *
thread_template (void *args)
{
  struct thread_info *t_info;
  void *status;

  t_info = (struct thread_info *)args;

  status = t_info->ti_routine (t_info->ti_args);
  
  unclone (status);
  /* We should not get to this point */
  return NULL;
}
/* ==================================== */
int
thread_create (void * (*thread_routine)(void *args), void *args)
{
  pid_t tid;
  uint8_t *aligned_stack;
  struct thread_info *t_info;

  if (first_time) {
    thread_init ();
    first_time = false;
  }

  t_info = thread_create_empty ();
  if (!t_info) {
    cprintf ("thread_create: maximum number of threads have been created!\n", 0x4);
    return -1;
  }

  t_info->ti_routine = thread_routine;
  t_info->ti_args    = args;

  t_info->ti_stack   = (uint8_t *)malloc (3*4096);
  if (!t_info->ti_stack) {
    cprintf ("thread_create: failed to alloc memory for thread's stack!\n", 0x4);
    return -1;
  }

  aligned_stack = (uint8_t*)ALIGN (((uint64_t)t_info->ti_stack), 0x1000);

  tid = clone (&thread_template, t_info, aligned_stack, 4096);

  if (tid < 0) {
    thread_destroy (t_info);
  }

  t_info->ti_tid = tid;
  return tid >= 0 ? tid : -1;
}
/* ==================================== */
void
thread_join (int tid)
{
  struct thread_info *t_info;

  t_info = thread_get_info (tid);
  if (!t_info) {
    cprintf ("thread_join: didn't find a thread with id = %d\n", 0x4, tid);
    return;
  }

  waitpid (tid, NULL);
  thread_destroy (t_info);
}

