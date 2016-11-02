#include "common.h"

JOB_QUEUE *job_list;
int fd;
int server_id;

void error_and_die(char* msg){
	perror(msg);
	exit(0);
}

void require_number_of_slots(int *size){
	printf("Please enter the number slots you want in shared memory\n");
	char size_str[11];
	fgets(size_str, 11, stdin);
	int len = strlen(size_str);
	if (len > 11){
		*size = MAX_SLOTS;
		return;
	}
	size_str[len] = '\0';
	*size = atoi(size_str);
	if ((*size <= 0) || (*size > MAX_SLOTS))
		error_and_die("Input invalid. Number of slots should range from 1 to 999999999");
}

void setup_shared_mem(){
	printf("Initializing shared memory region\n");
	fd =  shm_open("/myshm", O_CREAT | O_RDWR, 0666);
	if (fd < 0)
		error_and_die("shm_open");
	ftruncate(fd, sizeof(JOB_QUEUE));
}

void attach_share_mem(int size){
	job_list = (JOB_QUEUE *)mmap(0, sizeof(JOB_QUEUE) + (sizeof(JOB)*size), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (job_list == MAP_FAILED)
		error_and_die("mmap failed in attach_share_mem");
}

void init_semaphore(int size){
	sem_init(&job_list->mutex, 1, 1);
	sem_init(&job_list->full, 1, 0);
	sem_init(&job_list->empty, 1, size);
	job_list->id = 0;
	job_list->start = 0;
	job_list->end = 0;
	job_list->size = size;
	job_list->jobs = malloc(sizeof(JOB)*size);
}

void take_a_job(JOB job){
	sem_wait(&job_list->empty);
	sem_wait(&job_list->mutex);
	job = job_list->jobs[job_list->start];
	job_list->start = (job_list->start+1) % (job_list->size);
}

void print_a_msg(JOB job){
	sleep(job.duration);
	printf("Printer %d finishes printing %d pages from client %d\n", server_id, job.duration, job.source);
}
		
void go_sleep(JOB job){
	sem_post(&job_list->mutex);

}


int main(int argc, char *argv[]){
	int size;
	JOB job;

	if (argc != 1){
		printf("Incorrect number of arguments\n");
		printf("Usage: ./server\n");	
		exit(1);
	}


	fd = shm_open("/myshm", O_RDWR, 0666);	
	printf("fd before %d\n", fd);
	if (fd < 0){
		require_number_of_slots(&size);
		setup_shared_mem();
		attach_share_mem(size);
		init_semaphore(size);
	}else{
		attach_share_mem(size);
	}
	job_list->id = job_list->id + 1;
	server_id = job_list->id;
	printf("size is %d\n", job_list->size);
	// main loop
	while(1){
		// take_a_job(&job);
		// print_a_msg(&job);
		// go_sleep(&job);
	}
	printf("done\n");
	return 0;
}