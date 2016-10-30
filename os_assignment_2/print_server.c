#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_NUM_OF_JOBS 10

struct job_queue{
	int index;
	int jobs[MAX_NUM_OF_JOBS];
};

void error_and_die(char* msg){
	perror(msg);
	exit(0);
}

int main(){

	int shm_id = shm_open("data", O_CREAT | O_RDWR, 0666);
	if (shm_id < 0){
		error_and_die("shm_open failed");
	}

	struct job_queue *job_list;
	job_list = mmap(0, sizeof(struct job_queue), PROT_WRITE | PROT_READ, MAP_SHARED, shm_id, 0);

	if (job_list == MAP_FAILED){
		error_and_die("mmap failed");
	}

	job_list->jobs[0] = 1;
	job_list->jobs[1] = 2;
	job_list->jobs[2] = 3;


	while(1){
		printf("waiting\n");
		sleep(1);
	}
	printf("done\n");
	return 0;
}