#include "common.h"

JOB_QUEUE *job_list;
int fd;
int client_id;

void error_and_die(char* msg){
	printf("%s\n", msg);
	exit(0);
}

void setup_shared_mem(){
	printf(ANSI_COLOR_YELLOW "Connecting to server\n" ANSI_COLOR_RESET);
	fd =  shm_open("/myshm", O_RDWR, 0666);
	if (fd < 0)
		error_and_die(ANSI_COLOR_RED "shm_open failed" ANSI_COLOR_RESET);
	ftruncate(fd, sizeof(JOB_QUEUE));
}

void attach_share_mem(){

	job_list = (JOB_QUEUE *)mmap(0, sizeof(JOB_QUEUE), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (job_list == MAP_FAILED){
		error_and_die(ANSI_COLOR_RED "mmap failed");
	}
}

void get_job_params(int argc, char *argv[], int *duration){
	if (argc != 2){
		printf(ANSI_COLOR_RED "Incorrect number of arguments\n" ANSI_COLOR_RESET);
		printf(ANSI_COLOR_RED "Usage: ./client <number of pages>\n" ANSI_COLOR_RESET);	
		printf(ANSI_COLOR_RED "Each page takes one second to print\n" ANSI_COLOR_RESET);
		exit(1);
	}

	*duration = atoi(argv[1]);
	if (*duration <=0 || *duration >= 1000)
		error_and_die(ANSI_COLOR_RED "Page number should range 1 to  999" ANSI_COLOR_RESET);
}

void create_job(JOB *job, int duration){
	(*job).duration = duration;
	(*job).source = client_id;
}

void put_a_job(JOB job){
	if (job_list->current_size >= job_list->size)
		printf(ANSI_COLOR_YELLOW "Currently the queue is full, waiting for an empty slot...\n" ANSI_COLOR_RESET);
	sem_wait(&job_list->empty);
	sem_wait(&job_list->mutex);
	job_list->jobs[job_list->start].duration = job.duration;
	job_list->jobs[job_list->start].source = job.source;
	job_list->start = (job_list->start + 1) % (job_list->size);
	job_list->current_size = job_list->current_size+1;
	printf("Client %d has %d pages to print, puts request in Buffer\n", client_id, job.duration);
	sem_post(&job_list->mutex);
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
	job_list->c_id = job_list->c_id + 1;
	client_id = job_list->c_id;
	create_job(&job, duration);
	put_a_job(job);
	release_share_mem();	

	return 0;
}