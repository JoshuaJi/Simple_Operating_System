#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/shm.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define MAX_SLOTS 999999999

typedef struct JOB{
	int duration;
	int source;
} JOB;

typedef struct JOB_QUEUE{
	int s_id;
	int c_id;
	int start;
	int end;
	int size;
	int current_size;
	JOB jobs[1];
	sem_t mutex;
	sem_t full;
	sem_t empty;
} JOB_QUEUE;
