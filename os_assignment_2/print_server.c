#include "common.h"

job_queue *job_list;
int fd;

void error_and_die(char* msg){
	perror(msg);
	exit(0);
}

void setup_shared_mem(){
	printf("Initializing shared memory region\n");
	fd =  shm_open("/myshm", O_CREAT | O_RDWR, 0666);
	if (fd < 0)
		error_and_die("shm_open");
	ftruncate(fd, sizeof(job_queue));
}

void attach_share_mem(int size){
	job_list = (job_queue *)mmap(0, sizeof(job_queue + (sizeof int)*size), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (job_list == MAP_FAILED)
		error_and_die("mmap failed");
}

void init_semaphore(int size){
	sem_init(&job_list->mutex, 1, 1);
	sem_init(&job_list->job, 1, size);
	job_list->id = 0;
	job_list->index = 0;
	job_list->jobs = malloc((sizeof int)*size);
}

int main(int argc, int*argv){
	int size;

	if (argc != 2){
		printf("Incorrect number of arguments\n");
		printf("Usage: ./server <number of slots>\n");	
		exit(1);
	}

	size = atoi(argv[1]);
	if (size <= 0){
		printf("Number of slots should be at least 1\n");
		exit(1);
	}

	fd = shm_open("/myshm", O_RDWR, 0666);	
	printf("fd before %d\n", fd);
	if (fd < 0){
		setup_shared_mem();
		attach_share_mem(size);
		init_semaphore(size);
	}else{
		attach_share_mem(size);
	}





	// main loop
	while(1){
		printf("index after %d\n", job_list->index);
		sleep(1);
	}
	printf("done\n");
	return 0;
}