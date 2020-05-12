#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>

//shared memory example very similar to: https://www.geeksforgeeks.org/posix-shared-memory-api/
//but it uses fork and functions in the same file to make it happen rather than a file per use case.
//Now it also implements the real producer consumer problem

const int SIZE		=	4096;
const char * name	=	"OS";
const char * full_name	=	"full";
const char * empty_name	=	"empty";
int shm_fd;

//semaphores:
sem_t *full, *empty;

void producer(void){

	printf("Inicializando productor\n");

	void * ptr;
	ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);


	sem_wait(full);
	//generate random number:
	int r = random();
	char rs[100];
	sprintf(rs, "%d", r);
	strcat(rs, "random number generated:");

	sprintf(ptr, "%s", rs); 
	ptr += strlen(rs);
	sem_post(empty);
	
}

void consumer(void){

	printf("Inicializando consumidor\n");

	sem_wait(empty);
	void * ptr;

	ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);

	printf("%s\n", (char*)ptr);
	sem_post(full);


}

int main(){

	full = sem_open(full_name, O_CREAT, 10);
	empty = sem_open(empty_name, O_CREAT, 0);
	
	if(full == SEM_FAILED){
		perror("sem_open(full)");
		exit(-1);
	}
	if(empty == SEM_FAILED){
		perror("sem_open(empty)");
		exit(-1);
	}

	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);

	ftruncate(shm_fd, SIZE);


	if(!fork())
		producer();
	else if(!fork())
		consumer();

	while(wait(NULL) > 0);

	sem_destroy(empty);
	sem_destroy(full);
	shm_unlink(name);

	return 0;
	
}
