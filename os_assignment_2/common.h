#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>

typedef struct job_queue{
	int id;
	int index;
	int *jobs;
	sem_t mutex;
	sem_t job;
} job_queue;
