#include "common.h"

JOB_QUEUE *job_list;
int fd;
int server_id;

void error_and_die(char* msg){
	printf("%s\n", msg);
	exit(0);
}

void sanity_check(int *size, int argc, char *argv[]){
	if (argc != 2){
		printf(ANSI_COLOR_RED "Incorrect number of arguments" ANSI_COLOR_RESET "\n");
		printf(ANSI_COLOR_RED "Usage: ./server <number of slots>" ANSI_COLOR_RESET "\n");	
		exit(1);
	}

	*size = atoi(argv[1]);
	if ((*size <= 0) || (*size > MAX_SLOTS))
		error_and_die(ANSI_COLOR_RED"Input invalid. Number of slots should range from 1 to 999999999" ANSI_COLOR_RESET "\n");
}

void setup_shared_mem(int size){
	printf("Initializing shared memory region" ANSI_COLOR_RESET "\n");
	fd =  shm_open("/myshm", O_CREAT | O_RDWR, 0666);
	if (fd < 0)
		error_and_die(ANSI_COLOR_RED"shm_open" ANSI_COLOR_RESET "\n");
	ftruncate(fd, sizeof(JOB_QUEUE) + (sizeof(JOB)*size));
}

void attach_share_mem(int size){
	job_list = (JOB_QUEUE *)mmap(0, sizeof(JOB_QUEUE) + (sizeof(JOB)*size), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (job_list == MAP_FAILED)
		error_and_die(ANSI_COLOR_RED"mmap failed in attach_share_mem" ANSI_COLOR_RESET "\n");
}

void init_semaphore(int size){
	sem_init(&job_list->mutex, 1, 1);
	sem_init(&job_list->full, 1, 0);
	sem_init(&job_list->empty, 1, size);
	job_list->s_id = 0;
	job_list->c_id = 0;
	job_list->start = 0;
	job_list->end = 0;
	job_list->size = size;
	job_list->current_size = 0;
}

void connect_shared_memory(int size){
	fd = shm_open("/myshm", O_RDWR, 0666);	
	if (fd < 0){
		setup_shared_mem(size);
		attach_share_mem(size);
		init_semaphore(size);
	}else{
		attach_share_mem(0);
	}
}

void assign_server_id(){
	job_list->s_id = job_list->s_id + 1;
	server_id = job_list->s_id;
	printf(ANSI_COLOR_YELLOW "buffer size is %d" ANSI_COLOR_RESET "\n", job_list->size);
	printf(ANSI_COLOR_YELLOW "server id: %d" ANSI_COLOR_RESET "\n", server_id);
}

void take_a_job(JOB *job){
	printf("waiting for full\n");
	sem_wait(&job_list->full);
    printf("waiting for mutex\n");
	sem_wait(&job_list->mutex);
	*job = job_list->jobs[job_list->end];
	job_list->end = (job_list->end+1) % (job_list->size);
	job_list->current_size = job_list->current_size-1;
}

void print_a_msg(JOB *job){
	printf("Printer %d starts printing %d pages from client %d\n", server_id, (*job).duration, (*job).source);
	sem_post(&job_list->mutex);
	sem_post(&job_list->empty);
}
		
void go_sleep(JOB *job){
	sleep((*job).duration);
	printf("Printer %d finishes printing %d pages from client %d\n", server_id, (*job).duration, (*job).source);
	if (job_list->current_size == 0)
		printf(ANSI_COLOR_YELLOW "No request in buffer, Printer sleeps" ANSI_COLOR_RESET "\n");
}

int main(int argc, char *argv[]){
	int size;
	JOB job;

	sanity_check(&size, argc, argv);
	connect_shared_memory(size);
	assign_server_id();

	// main loop
	while(1){
		take_a_job(&job);
		print_a_msg(&job);
		go_sleep(&job);
	}
	return 0;
}