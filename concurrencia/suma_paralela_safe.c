#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "barrera.h"

#define DEBUG 1

int NELEMS, NTHREADS;
barrier_t mybarrier;
int * elementos;
int * tmp;


void * parallel_sum(void * data){

	int pos = *((int *) data);
	int e = NELEMS/NTHREADS;
	if(DEBUG)printf("voy a sumar %d elementos y poner el resultado de mi suma en la posicion: %d del vector\n", e, pos);
	int suma_parcial = 0;
	for(int i = pos * e; i < (pos + 1) * e; i++)
		suma_parcial += elementos[i];

	if(DEBUG)printf("el resultado de mi suma parcial ha sido: %d\n", suma_parcial);

	tmp[pos] = suma_parcial;

	barrier_wait(&mybarrier);

}

void * copy_arrays(void * data){
	int pos = *((int *) data);
	elementos[pos] = tmp[pos];
	barrier_wait(&mybarrier);
}


int main(){

	srand(time(0));

	NELEMS = 64;
	NTHREADS = 8;

	if(NELEMS % NTHREADS != 0){
		printf("el numero de elementos debe ser multiplo de el numero de hilos\n");
		return 0;
	}

	elementos = (int *)malloc(NELEMS * sizeof(int));
	tmp = (int *)malloc(NELEMS/NTHREADS * sizeof(int));

	
	int res_secuencial = 0;
	if(DEBUG)printf("Contenido del vector:\n");
	for(int i = 0; i < NELEMS; i++){
		elementos[i] = rand() % 101;
		//printf("%d ", elementos[i]);
		res_secuencial += elementos[i];
	}


	printf("\n\n");

	pthread_t ids[NTHREADS];
	int short_ids[NTHREADS];

	while(NTHREADS >= 1){
		barrier_init(&mybarrier, NULL, NTHREADS + 1);
		if(DEBUG)printf("Numero de hilos a utilizar: %d\n", NTHREADS);
		for(int i = 0; i < NTHREADS; i++){
			short_ids[i] = i;
			pthread_create(&ids[i], NULL, parallel_sum, &short_ids[i]);
		}
		barrier_wait(&mybarrier);

		barrier_init(&mybarrier, NULL, NTHREADS + 1);
		for(int i = 0; i < NTHREADS; i++){
			short_ids[i] = i;
			pthread_create(&ids[i], NULL, copy_arrays, &short_ids[i]);
		}
		barrier_wait(&mybarrier);

		NELEMS = NTHREADS;
		NTHREADS/=2;
	}

	int res_paralelo = elementos[0];
	printf("suma paralela: %d, suma secuencial %d\n", res_paralelo, res_secuencial);




}
