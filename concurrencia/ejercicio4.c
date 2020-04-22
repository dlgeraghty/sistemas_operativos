#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

bool ingredientes[3];

pthread_mutex_t mtx;
pthread_cond_t cond;

void * agente(void * data){

	while(true){

		pthread_mutex_lock(&mtx);

		while(ingredientes[0] || ingredientes[1] || ingredientes[2]) pthread_cond_wait(&cond, &mtx);

		int a = random() % 3;
		int b;
		do{b = random()%3;} while(a == b);
		printf("produzco %d y %d\n" , a , b );
		ingredientes[a] = true;
		ingredientes[b] = true;

		pthread_cond_broadcast(&cond);

		pthread_mutex_unlock(&mtx);
	}
}

void * fumadores(void * data){
	unsigned int * necesito = data;
	while(true){
		pthread_mutex_lock(&mtx);

		while(!ingredientes[necesito[0]] || !ingredientes[necesito[1]])
			pthread_cond_wait(&cond, &mtx);

		ingredientes[necesito[0]] = false;
		ingredientes[necesito[1]] = false;

		pthread_cond_broadcast(&cond);

		pthread_mutex_unlock(&mtx);

		printf("Fumo: %d y %d\n", necesito[0], necesito[1]);
		sleep(random() % 20);
	}
}

int main(){

	//inicializar ingredientes:
	for(int i = 0; i < 3; i++) ingredientes[i] = false;

	//arrays de ingredientes individuales para cada proceso:
	int ingredientes1[] = {0,1};
	int ingredientes2[] = {1,2};
	int ingredientes3[] = {0,2};

	//inicializar sincronizacion:
	pthread_mutex_init(&mtx,	NULL);
	pthread_cond_init(&cond,	NULL);

	//hilos:
	pthread_t ag, f1, f2, f3;
	pthread_create(&ag,	NULL,	agente,		NULL);
	pthread_create(&f1,	NULL,	fumadores,	ingredientes1);
	pthread_create(&f2,	NULL,	fumadores,	ingredientes2);
	pthread_create(&f3,	NULL,	fumadores,	ingredientes3);


	pthread_join(ag,	NULL);
	pthread_join(f1,	NULL);
	pthread_join(f2,	NULL);
	pthread_join(f3,	NULL);

	//destruir sincronizacion:
	pthread_mutex_destroy(&mtx);
	pthread_cond_destroy(&cond);

	return 0;
}
