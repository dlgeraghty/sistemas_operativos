#include <pthread.h>
#include <unistd.h>

typedef struct{
	int i;
	pthread_mutex_t mtx;
	pthread_cond_t cond;
} barrier_t;

void barrier_wait(barrier_t * b){

	pthread_mutex_lock(&(b->mtx));
	if(b->i > 0){
		b->i--;
		//printf("valor de i: %d\n", b->i);
		pthread_cond_wait(&(b->cond), &(b->mtx));
	}
	else{
		pthread_cond_broadcast(&(b->cond));
		//printf("voy a hacer el broadcast, por que i es: %d\n", b->i);
	}

	pthread_mutex_unlock(&(b->mtx));
}




void barrier_init(barrier_t * b, barrier_t * d, int val){

	if(d != NULL){
		b->i = d->i;
		b->mtx = d->mtx;
		b->cond = d->cond;
	}else{
		pthread_mutex_init(&(b->mtx), NULL);
		pthread_cond_init(&(b->cond), NULL);
		b->i = val - 1;
	}

}

void barrier_destroy(barrier_t * b){

	pthread_mutex_destroy(&(b->mtx));
	pthread_cond_destroy(&(b->cond));

}
