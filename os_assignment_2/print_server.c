#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>

#define MAX_NUM_OF_JOBS 10

typedef struct job_queue{
	int id;
	int index;
	int jobs[MAX_NUM_OF_JOBS];
	sem_t mutex;
	sem_t job;
} job_queue;

job_queue *job_list;
int fd;

void error_and_die(char* msg){
	perror(msg);
	exit(0);
}

// job_queue *create_job_list(){
// 	job_queue *job_list = malloc(sizeof(job_queue));
// 	job_list->index = 0;
// 	for (int i = 0; i < MAX_NUM_OF_JOBS; i++){
// 		job_list->jobs[i] = 0;
// 	}
// 	return job_list;
// }

void attach_share_memory(){
	job_list = (job_queue *)mmap(0, sizeof(job_queue), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (job_list == MAP_FAILED){
		error_and_die("mmap failed");
	}
}

int main(){


	// Initial shared memory
	int status = shm_unlink("/myshm");
	if (status == -1){
		error_and_die("shm_unlink");
	}
	fd = shm_open("/myshm", O_RDWR, 0666);	
	if (fd < 0){
		printf("Initializing shared memory region\n");
		fd =  shm_open("/myshm", O_CREAT, 0666);

		if (fd < 0)
		{
			error_and_die("shm_open 1");
		}


		ftruncate(fd, sizeof(job_queue));

		attach_share_memory();

	// Initial semaphore if not already
		if (job_list->id == 0){
			printf("NULL\n");
		}else{
			printf("%d\n", job_list->index);
		}
	}

	printf("%d\n", fd);



	// main loop
	while(1){
		printf("waiting\n");
		sleep(1);
	}
	printf("done\n");
	return 0;
}