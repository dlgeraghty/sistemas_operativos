#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct{
	int a, b;
}datos;

int resultado, hilos;
sem_t mtx, join;
datos * vector;

void * producto( void * args ){

	datos d = (*(datos*)args);


	sem_wait(&mtx);
	resultado += d.a * d.b;
	hilos--;
	if(hilos <= 0)
		sem_post(&join);
	sem_post(&mtx);

}

int main(int argc, char * args[]){

	int N, R, sequential;
	pthread_t * threads;

	srand(time(NULL));

	//argument test:
	if(argc != 3){
		printf("usage: ./producto_vectorial number_of_parallel_instances, max_number_in_operations");
		exit(0);
	}

	sscanf(args[1], "%d" , &N);
	sscanf(args[2], "%d" , &R);

	//memory allocation and initialization of variables: 
	vector		=	malloc(N * sizeof(datos));
	threads 	= 	(pthread_t* )malloc(N * sizeof(pthread_t));
	sequential	= 	0;
	resultado	=	0;
	hilos		=	N;
	sem_init(&mtx,	0,	1);
	sem_init(&join,	0,	0);

	
	//fill vector with random numbers using the datos structure and calculate the sequential sum and print it:
	printf("el vector es: \n");
	for(int i = 0; i < N; i++){
		vector[i].a = rand() % R;
		vector[i].b = rand() % R;
		printf("%d*%d + ", vector[i].a , vector[i].b);
		sequential += vector[i].a * vector[i].b;
	}
	printf("\n");

	for(int i = 0; i < N; i++)
		pthread_create(&threads[i], NULL , producto, &vector[i]);

	sem_wait(&join);

	printf("el resultado paralelo es: %d y el resultado secuencial es: %d\n", resultado, sequential);

	//free resources:
	free(vector);
	sem_destroy(&mtx);
	sem_destroy(&join);
	
	
	return 0;

}
