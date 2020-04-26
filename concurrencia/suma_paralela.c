#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "barrera.h"


int NELEMS, NTHREADS;
barrier_t mybarrier;
int * elementos;

void * parallel_sum(void * data){

	int pos = *((int *) data);
	printf("voy a poner el resultado de mi suma en la posicion: %d del vector\n", pos);
	barrier_wait(&mybarrier);

}

int main(){

	NELEMS = 16;
	NTHREADS = 4;

	elementos = (int *)malloc(NELEMS * sizeof(int));

	pthread_t ids[NTHREADS];
	int short_ids[NTHREADS];


	while(NTHREADS > 1){
		barrier_init(&mybarrier, NULL, NTHREADS + 1);
		printf("Numero de hilos a utilizar: %d\n", NTHREADS);
		for(int i = 0; i < NTHREADS; i++){
			short_ids[i] = i;
			pthread_create(&ids[i], NULL, parallel_sum, &short_ids[i]);
		}
		barrier_wait(&mybarrier);

		NTHREADS/=2;
	}



}
