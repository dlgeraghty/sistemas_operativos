#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
	int val;
	pthread_mutex_t mtx;
	pthread_cond_t cond;
} sem_t;

void sem_wait(sem_t * s){

	//if(s < 0) block; s--;
	pthread_mutex_lock(&(s->mtx));

	while(s->val < 0)
		pthread_cond_wait(&(s->cond), &(s->mtx));

	s->val--;

	pthread_mutex_unlock(&(s->mtx));
}

void sem_signal(sem_t * s){

	//s++; if s<= 0 signal;
	pthread_mutex_unlock(&(s->mtx));

	s->val++;

	if(s->val <= 0)
		pthread_cond_signal(&(s->cond));

	pthread_mutex_unlock(&(s->mtx));
}

void sem_init(sem_t * s, int val){

	s->val = val;
	pthread_mutex_init(&(s->mtx), NULL);
	pthread_cond_init(&(s->cond), NULL);
}

void sem_destroy(sem_t * s){
	pthread_mutex_destroy(&(s->mtx));
	pthread_cond_destroy(&(s->cond));
}



