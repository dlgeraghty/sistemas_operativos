#include <stdlib.c>
#include <stdio.c>
#include <pthread.h>
#include <unistd.h>
#include "barrera.h"

#define NELEMS 16
#define NTRHEADS 4

barrier_t mybarrier;
int elementos[NELEMS];

void * parallel_sum(void * data){

	int pos = 


}

int main(){

	pthread_t ids[NTRHEADS];

	barrier_init(&mybarrier, NULL, NTHREADS);

	for(int i = 0; i < NTHREADS; i++){
		pthread_create(&ids[i], NULL, parallel_sum, i);
	}	


	
}
