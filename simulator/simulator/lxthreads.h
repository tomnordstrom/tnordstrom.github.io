#ifndef _LXTHREADS_H_
#define _LXTHREADS_H_

#include <pthread.h>
#include <stdbool.h>

typedef struct 
{
	pthread_t thread;
	pthread_mutex_t mutex;
	pthread_cond_t  condition; 
	bool runflag;
} lxthread_t;

int lxthread_Init(void *(*start_routine) (void *));	// create and starts a new thread. Called from master thread.
void lxthread_Release(int handle);					// Releases the controlled thread from waiting state. Called from master thread.
void lxthread_WaitFinished(int handle);				// Wait execution on master thread  until controlled thread is in waiting state. Called from master thread.
void lxthread_WaitNp(int ticks);					// Wait execution until lxthread_Release is processed from master thread. Called from controlled thread.

// --- periodic timer stuff --------------

struct periodic_info
{
	int timer_fd;
	unsigned long long wakeups_missed;
};
int make_periodic(unsigned int period, struct periodic_info *info);
void wait_period(struct periodic_info *info);

#endif
