#include <pthread.h>
#include <unistd.h>

typedef struct{
	int i;
	pthread_mutex_t mtx;
	pthread_cond_t cond;
} barrier_t;

void barrier_wait(barrier_t * b){

	pthread_mutex_lock(&(b->mtx));
	b->i--;
	if(b->i > 0)
		pthread_cond_wait(&(b->cond), &(b->mtx));
	else
		pthread_cond_broadcast(&(b->cond));
	pthread_mutex_unlock(&(b->mtx));
}




void barrier_init(barrier_t * b, barrier_t * d, int val){

	if(d != NULL){
		b->i = d->i;
		b->mtx = d->mtx;
		b->cond = d->cond;
	}else{
		b->i = val;
		pthread_mutex_init(&(b->mtx), NULL);
		pthread_cond_init(&(b->cond), NULL);
	}

}

void barrier_destroy(barrier_t * b){

	pthread_mutex_destroy(&(b->mtx));
	pthread_cond_destroy(&(b->cond));

}
