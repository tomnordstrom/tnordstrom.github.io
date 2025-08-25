#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/timerfd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>
#include "lxthreads.h"

int threadCnt = 0;
lxthread_t lxthreads[20]; // store all threads here for internal use

int lxthread_Init(void *(*start_routine) (void *))
{
	if (threadCnt >= 20)
		return -1;
	pthread_mutex_init( &(lxthreads[threadCnt].mutex), NULL);
	pthread_create(&(lxthreads[threadCnt].thread), NULL, start_routine, NULL);
	pthread_mutex_lock(&lxthreads[threadCnt].mutex);
	lxthreads[threadCnt].runflag = true;
	++threadCnt;
	return threadCnt - 1;
}

void lxthread_Release(int handle)
{
	pthread_mutex_lock(&(lxthreads[handle].mutex));
	lxthreads[handle].runflag = false;
	pthread_cond_signal(&(lxthreads[handle].condition));
	pthread_mutex_unlock(&(lxthreads[handle].mutex));
	while (!lxthreads[handle].runflag)	// wait until mutex is locked again by thread
		usleep(0);
}

void lxthread_WaitFinished(int handle)
{
	pthread_mutex_lock(&(lxthreads[handle].mutex));
	pthread_mutex_unlock(&(lxthreads[handle].mutex));
}

void lxthread_WaitNp(int ticks)
{
	int handle;
	for (handle = 0; handle < threadCnt; ++handle)
		if (pthread_self() == lxthreads[handle].thread)
			break;
	for (int i = 0; i < ticks; ++i)
	{
		pthread_cond_wait(&(lxthreads[handle].condition), &(lxthreads[handle].mutex));
		lxthreads[handle].runflag = true;
	}
}

int make_periodic(unsigned int period, struct periodic_info *info)
{
	int ret;
	unsigned int ns;
	unsigned int sec;
	int fd;
	struct itimerspec itval;

	/* Create the timer */
	fd = timerfd_create(CLOCK_MONOTONIC, 0);
	info->wakeups_missed = 0;
	info->timer_fd = fd;
	if (fd == -1)
		return fd;

	/* Make the timer periodic */
	sec = period / 1000000;
	ns = (period - (sec * 1000000)) * 1000;
	itval.it_interval.tv_sec = sec;
	itval.it_interval.tv_nsec = ns;
	itval.it_value.tv_sec = sec;
	itval.it_value.tv_nsec = ns;
	ret = timerfd_settime(fd, 0, &itval, NULL);
	return ret;
}

void wait_period(struct periodic_info *info)
{
	unsigned long long missed;
	int ret;

	/* Wait for the next timer event. If we have missed any the
	   number is written to "missed" */
	ret = read(info->timer_fd, &missed, sizeof(missed));
	if (ret == -1) {
		perror("read timer");
		return;
	}
	info->wakeups_missed += missed;
}


