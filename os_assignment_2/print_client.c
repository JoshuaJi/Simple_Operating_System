#include "common.h"

job_queue *job_list;
int fd;

void error_and_die(char* msg){
	perror(msg);
	exit(0);
}

void attach_share_memory(){
	job_list = (job_queue *)mmap(0, sizeof(job_queue), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (job_list == MAP_FAILED){
		error_and_die("mmap failed");
	}
}

int main(){

	// Initial shared memory
	fd = shm_open("/myshm", O_RDWR, 0666);
	if (fd < 0){
		error_and_die("shm_open failed");
	}

    ftruncate(fd, sizeof(job_queue));

	attach_share_memory();


	printf("%d\n", job_list->index);
	shmdt(job_list);

	return 0;
}