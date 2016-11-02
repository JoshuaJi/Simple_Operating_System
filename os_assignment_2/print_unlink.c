#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>

#define MAX_NUM_OF_JOBS 10



int main(){


	// Initial shared memory
	int status = shm_unlink("/myshm");
	if (status == -1){
		printf("shm_unlink -1 \n");
	}
	
	return 0;
}