#ifndef _MEASURE_TIME_H_
#define _MEASURE_TIME_H_

#include <stdbool.h>
#include <sys/time.h>

#define ID_0 0
#define ID_1 1
#define ID_2 2
#define ID_3 3
#define ID_4 4

#define MTIME_MEASURE 0
#define MTIME_START 1

#define WITHOUT_DEBUG 0
#define WITH_DEBUG 1

void mtimeInit(int mtimeId,int limitValue);
int mtimeMeasureTime(int mtimeId, bool start, bool debug);

void timespec_diff(struct timespec *start, struct timespec *stop,struct timespec *result);
bool firstIsGreatest(struct timespec *a, struct timespec *b);

#endif
