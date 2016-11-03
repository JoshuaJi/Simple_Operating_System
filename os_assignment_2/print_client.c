#include "common.h"

JOB_QUEUE *job_list;
int fd;
int client_id;

void error_and_die(char* msg){
	perror(msg);
	exit(0);
}

void setup_shared_mem(){
	printf("Connecting to server\n");
	fd =  shm_open("/myshm", O_RDWR, 0666);
	if (fd < 0)
		error_and_die("shm_open");
	ftruncate(fd, sizeof(JOB_QUEUE));
}

void attach_share_mem(){

	job_list = (JOB_QUEUE *)mmap(0, sizeof(JOB_QUEUE), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (job_list == MAP_FAILED){
		error_and_die("mmap failed");
	}
}

void get_job_params(int argc, char *argv[], int *duration){
	if (argc != 2){
		printf("Incorrect number of arguments\n");
		printf("Usage: ./client <number of pages>\n");	
		printf("Each page takes one second to print\n");
		exit(1);
	}

	*duration = atoi(argv[1]);
	if (*duration <=0 || *duration >= 1000)
		error_and_die("Page number should range 1 to  999");
}

JOB create_job(int duration, int id){
	JOB temp_job;
	temp_job.duration = duration;
	temp_job.source = id;
	return temp_job;
}

void put_a_job(JOB job){
	sem_wait(&job_list->empty);

	sem_wait(&job_list->mutex);
	if (job_list->current_size == job_list->size){
		job_list->id = job_list->id - 1;
		error_and_die("Currently the job queue is full, try again later");
	}
	job_list->jobs[job_list->start] = job;
	job_list->start = (job_list->start + 1) % (job_list->size);
	job_list->current_size = job_list->current_size+1;
	printf("Client %d has %d pages to print, puts request in Buffer\n", client_id, job.duration);
	sem_post(&job_list->mutex);
	sem_post(&job_list->empty);
	sem_post(&job_list->full);
}

void release_share_mem(){
	shmdt(job_list);
}

int main(int argc, char* argv[]){
	JOB job;
	int duration;

	setup_shared_mem();
	attach_share_mem();
	get_job_params(argc, argv, &duration); 
	job_list->id = job_list->id + 1;
	client_id = job_list->id;
	job = create_job(duration, client_id);
	put_a_job(job);
	release_share_mem();	

	return 0;
}