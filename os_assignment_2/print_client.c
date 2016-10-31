#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>

#define MAX_NUM_OF_JOBS 10

typedef struct job_queue{
	int index;
	int jobs[MAX_NUM_OF_JOBS];
	sem_t mutex;
	sem_t job;
} job_queue;

void error_and_die(char* msg){
	perror(msg);
	exit(0);
}

job_queue *create_job_list(){
	job_queue *job_list = malloc(sizeof(job_queue));
	job_list->index = 0;
	for (int i = 0; i < MAX_NUM_OF_JOBS; i++){
		job_list->jobs[i] = 0;
	}
	return job_list;
}

int main(){

	// Initial shared memory
	int fd = shm_open("data", O_RDWR, 0666);
	if (fd < 0){
		error_and_die("shm_open failed");
	}

    ftruncate(fd, sizeof(job_queue));

	job_queue *job_list;
	job_list = mmap(0, sizeof(job_queue), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);

	if (job_list == MAP_FAILED){
		error_and_die("mmap failed");
	}

	// 

	printf("%d\n", job_list->jobs[0]);
	printf("%d\n", job_list->jobs[1]);
	printf("%d\n", job_list->jobs[2]);
	printf("done\n");
	return 0;
}