#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>

//shared memory example very similar to: https://www.geeksforgeeks.org/posix-shared-memory-api/
//but it uses fork and functions in the same file to make it happen rather than a file per use case.

const int SIZE		=	4096;
const char * name	=	"OS";
const char * message1	=	"Hello";
const char * message2	=	"World";
int shm_fd;

void producer(void){

	void * ptr;
	ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

	sprintf(ptr, "%s", message1); 
	ptr += strlen(message1);
	sprintf(ptr, "%s", message2);
	ptr += strlen(message2);
	
}

void consumer(void){

	void * ptr;

	ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);

	printf("%s\n", (char*)ptr);

	shm_unlink(name);


}

int main(){

	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);

	ftruncate(shm_fd, SIZE);


	if(!fork())
		producer();
	else if(!fork())
		consumer();

	while(wait(NULL) > 0);

	return 0;
	
}
