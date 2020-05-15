#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/mman.h>

typedef struct{
	int a, b;
}datos;

const int SIZE		=	4096;
const char * name	=	"OS";
const char * sem_name	=	"semaphore";
sem_t * mtx;
int shm_fd;

void producto(void * , sem_t * );

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
	
	//fill vector with random numbers using the datos structure and calculate the sequential sum and print it:
	printf("el vector es: \n");
	for(int i = 0; i < N; i++){
		vector[i].a = rand() % R;
		vector[i].b = rand() % R;
		printf("%d*%d + ", vector[i].a , vector[i].b);
	//	sequential += vector[i].a * vector[i].b;
		sequential += i;
	}
	printf("\n");


	for(int i = 0; i < N; i++)
		if(fork() == 0)
			producto(&vector[i], mtx);
		
	while(wait(NULL)>0);
	printf("el resultado paralelo es: %d y el resultado secuencial es: %d\n", 0, sequential);

	//free resources:
	free(vector);
	sem_destroy(mtx);
	shm_unlink(name);
	
	return 0;

}

void producto(void * args, sem_t * x){

	datos d = (*(datos*)args);

//	resultado += d.a * d.b;
	
	
	sem_wait(x);
	printf("my arguments are: %d and %d\n", d.a, d.b);
	sem_post(x);

	exit(0);
}
