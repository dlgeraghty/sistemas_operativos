#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/types.h>

typedef struct{
	int a, b;
}datos;

const int SIZE		=	4096;
const char * name	=	"OS";
const char * sem_name	=	"semaphore";
sem_t * mtx;
int shm_fd;

void producto(void * , sem_t * );

struct shared_data{
	int data;
};

int main(int argc, char * args[]){

	datos * vector;
	int N, R, sequential, hilos;

	srand(time(NULL));

	//argument test:
	if(argc != 3){
		printf("usage: ./producto_vectorial number_of_parallel_instances, max_number_in_operations");
		exit(0);
	}

	sscanf(args[1], "%d" , &N);
	sscanf(args[2], "%d" , &R);

	//memory allocation and initialization of variables: 
	vector		=	(datos *)malloc(N * sizeof(datos));
	sequential	= 	0;
	hilos		=	N;
	shm_fd		=	shm_open(name, O_CREAT|O_RDWR, 0666);
	mtx		=	sem_open(sem_name, O_CREAT, 1);

	sem_init(mtx, 1, 1);
	ftruncate(shm_fd, sizeof(struct shared_data));
	
	//fill vector with random numbers using the datos structure and calculate the sequential sum and print it:
	printf("el vector es: \n");
	for(int i = 0; i < N; i++){
		vector[i].a = rand() % R;
		vector[i].b = rand() % R;
		printf("%d*%d + ", vector[i].a , vector[i].b);
		sequential += vector[i].a * vector[i].b;
	}
	printf("\n");

	//"initialize" the shm value, in this case, just put a 0 in it
	struct shared_data * ptr = (struct shared_data *) mmap(0, sizeof(shared_data), PROT_WRITE, MAP_SHARED, shm_fd, 0);
	ptr->data = 0;

	for(int i = 0; i < N; i++)
		if(fork() == 0)
			producto(&vector[i], mtx);
		
	while(wait(NULL)>0);
	ptr = (struct shared_data *) mmap(0, sizeof(shared_data), PROT_WRITE, MAP_SHARED, shm_fd, 0);
	printf("el resultado paralelo es: %d y el resultado secuencial es: %d\n", ptr->data, sequential);

	//free resources:
	free(vector);
	sem_destroy(mtx);
	shm_unlink(name);
	
	return 0;

}

void producto(void * args, sem_t * x){

	datos d = (*(datos*)args);

	struct shared_data * ptr = (struct shared_data *) mmap(0, sizeof(shared_data), PROT_WRITE, MAP_SHARED, shm_fd, 0);
	
	sem_wait(x);
	printf("my arguments are: %d and %d\n", d.a, d.b);
	ptr->data += d.a * d.b;
	sem_post(x);

	exit(0);
}
