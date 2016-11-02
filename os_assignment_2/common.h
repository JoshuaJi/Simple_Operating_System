#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>

#define MAX_SLOTS 999999999

typedef struct JOB{
	int duration;
	int id;
	int source;
} JOB;

typedef struct JOB_QUEUE{
	int id;
	int start;
	int end;
	int size;
	JOB *jobs;
	sem_t mutex;
	sem_t full;
	sem_t empty;
} JOB_QUEUE;
