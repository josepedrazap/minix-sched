#include <minix/mthread.h>
#include "global.h"
#include "proto.h"


int mthread_barrier_init(barrier, h)
mthread_barrier_t *barrier;
int h;
{
  struct __mthread_barrier *b;

  if (barrier == NULL)
    return(EAGAIN);

  else if ((b = malloc(sizeof(struct __mthread_barrier))) == NULL) 
    return(ENOMEM);
  b->hilos_ = h;
  mthread_queue_init(&b->brr_queue);
  b->brr_owner = NO_THREAD;
  *barrier = (mthread_barrier_t) b;

  return(0);
}


int mthread_barrier_sync(barrier)
mthread_barrier_t *barrier; 
{
  struct __mthread_barrier *b;

  if (barrier == NULL)
    return(EINVAL);

  b = (struct __mthread_barrier *) *barrier;

  if (b->brr_owner == NO_THREAD) { /* Not locked */

      b->brr_owner = current_thread;
      b->hilos++;
      while(b->hilos != b->hilos_){
      }

      while(b->brr_owner != NO_THREAD){
        b->brr_owner = mthread_queue_remove(&b->brr_queue);
        mthread_unsuspend(b->brr_owner);
      }
 
  }else if (b->brr_owner == current_thread) {
    return(EDEADLK);
  } else {
  b->hilos++;
  mthread_queue_add(&b->brr_queue, current_thread);
  mthread_suspend(MS_MUTEX);
 
  }
  return(0);
}

__weak_alias(pthread_barrier_sync, mthread_barrier_sync)
__weak_alias(pthread_barrier_init, mthread_barrier_init)
