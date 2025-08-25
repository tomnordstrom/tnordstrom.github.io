
#include "measureTime.h"
#include <stdio.h>
#include <pthread.h>

typedef struct 
{
	struct timespec tpStart, tpStop, tpResult, tpMax, tpLimit;
	bool first;
	int aboveLimitCnt;
} mtime_t;

#define NO_OF_MTIME 5

mtime_t mtime[NO_OF_MTIME];

void mtimeInit(int mtimeId,int limitValue)
{
	if (mtimeId >= NO_OF_MTIME)
		return;
	mtime[mtimeId].first = true;
	mtime[mtimeId].tpMax.tv_sec = 0;
	mtime[mtimeId].tpMax.tv_nsec = 0;
	mtime[mtimeId].tpLimit.tv_sec = 0;
	mtime[mtimeId].tpLimit.tv_nsec = limitValue;
}

int mtimeMeasureTime(int mtimeId, bool start, bool debug)
{
	
	if (mtimeId >= NO_OF_MTIME)
		return 0;
/*		gettimeofday(&t1,0);
		printf("timeofday: %d %d\n",t1.tv_sec,t1.tv_usec);
		if (!first)  
		{
			timersub(&t1,&t1old,&tdiff);
			if (timercmp(&tdiff,&tdiffmax,>))
			{
				tdiffmax.tv_sec = tdiff.tv_sec;
				tdiffmax.tv_usec = tdiff.tv_usec;
			}
			printf("time: %d %d\n",t1.tv_sec,t1.tv_usec);
			printf("diff: %d %d max: %d %d\n",tdiff.tv_sec,tdiff.tv_usec,tdiffmax.tv_sec,tdiffmax.tv_usec);
		}
		t1old.tv_sec = t1.tv_sec;
		t1old.tv_usec = t1.tv_usec;
		first = false;
  */

		clock_gettime(CLOCK_MONOTONIC, &mtime[mtimeId].tpStop);

		if (!start && !mtime[mtimeId].first)
		{
			timespec_diff(&mtime[mtimeId].tpStart,&mtime[mtimeId].tpStop,&mtime[mtimeId].tpResult);
			if (firstIsGreatest(&mtime[mtimeId].tpResult,&mtime[mtimeId].tpMax))
			{
				mtime[mtimeId].tpMax.tv_sec = mtime[mtimeId].tpResult.tv_sec;
				mtime[mtimeId].tpMax.tv_nsec = mtime[mtimeId].tpResult.tv_nsec;
			}			
			if (firstIsGreatest(&mtime[mtimeId].tpResult,&mtime[mtimeId].tpLimit))
				++mtime[mtimeId].aboveLimitCnt;
			if (debug)
         {
				printf("mtimeId: %d diff: %d.%09d \t max: %d.%09d \t LimitCnt: %d\n",mtimeId,mtime[mtimeId].tpResult.tv_sec, mtime[mtimeId].tpResult.tv_nsec, mtime[mtimeId].tpMax.tv_sec, mtime[mtimeId].tpMax.tv_nsec,mtime[mtimeId].aboveLimitCnt);
            mtime[mtimeId].tpMax.tv_sec = 0;
            mtime[mtimeId].tpMax.tv_nsec = 0;
         }
		}
		mtime[mtimeId].tpStart.tv_sec = mtime[mtimeId].tpStop.tv_sec;
		mtime[mtimeId].tpStart.tv_nsec = mtime[mtimeId].tpStop.tv_nsec;
		mtime[mtimeId].first = false;
	return mtime[mtimeId].tpMax.tv_nsec;
}


void timespec_diff(struct timespec *start, struct timespec *stop,struct timespec *result)
{
    if ((stop->tv_nsec - start->tv_nsec) < 0)
	{
        result->tv_sec = stop->tv_sec - start->tv_sec - 1;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec + 1000000000;
    } else
	{
        result->tv_sec = stop->tv_sec - start->tv_sec;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec;
    }
    return;
}

bool firstIsGreatest(struct timespec *a, struct timespec *b)
{
    if (a->tv_sec == b->tv_sec)
        return a->tv_nsec > b->tv_nsec;
    else
        return a->tv_sec > b->tv_sec;
}
