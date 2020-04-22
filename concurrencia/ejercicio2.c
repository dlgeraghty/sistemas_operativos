#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_NUM 10

int numero;

int done_pares   = 0;
int done_impares = 0;

// turno = { IMPARES, PARES, CONSUMIDOR }
typedef enum {
  IMPARES, PARES, CONSUMIDOR
}type_turno;

type_turno turno = IMPARES;

// MUTEX m
pthread_mutex_t m;
// VC vcImpares, vcPares, vcConsumidor
pthread_cond_t cvImpares = PTHREAD_COND_INITIALIZER;
pthread_cond_t cvPares = PTHREAD_COND_INITIALIZER;
pthread_cond_t cvConsumidor = PTHREAD_COND_INITIALIZER;

void *Impares(void *data){
  int i;

  for( i=1; i <= 2*MAX_NUM; i+=2 ){
	// lock( m )
	pthread_mutex_lock(&m);
	// while turno != IMPARES
	while(turno != IMPARES)
		//   wait( vcImpares, m )
		pthread_cond_wait(&cvImpares, &m);
	numero=i;
	// signal( vcConsumidor )
	pthread_cond_signal(&cvConsumidor);
	// turno = CONSUMIDOR
	turno = CONSUMIDOR;
	// unlock( m )
	pthread_mutex_unlock(&m);
  }
  done_impares = 1;
}

void *Pares(void *data){
  int i;

  for( i=2; i <= 2*MAX_NUM; i+=2 ){
	// lock( m )
	pthread_mutex_lock(&m);
	// while turno != PARES
	while(turno != PARES)
		//   wait( vcPares, m )
		pthread_cond_wait(&cvPares, &m);
    numero=i;
        // signal( vcConsumidor )
	pthread_cond_signal(&cvConsumidor);
	// turno = CONSUMIDOR
	turno = CONSUMIDOR;
	// unlock( m )
	pthread_mutex_unlock(&m);
  }
  done_pares = 1;
}

void *Consumidor(void *data){

  while(!done_impares || !done_pares){
	// lock( m )
	pthread_mutex_lock(&m);
	// while turno != CONSUMIDOR
	while(turno != CONSUMIDOR)
		//   wait( vcConsumidor, m )
		pthread_cond_wait(&cvConsumidor, &m);

	printf("Dato: %d\n",numero);

        // if numero % 2 == 0
	if(numero % 2 == 0){
		//   turno = IMPARES
		turno = IMPARES;
		//   signal( vcImpares )
		pthread_cond_signal(&cvImpares);
	// else
	}else{
		//   turno = PARES
		turno = PARES;
		//   signal( vcPares )
		pthread_cond_signal(&cvPares);
	}
	// unlock( m )
	pthread_mutex_unlock(&m);
  }
  printf("Consumidor done!!\n");
}

int main(int argc, char *argv[]) {
  pthread_t pTh_par, pTh_impar, pTh_cons;

  pthread_mutex_init(&m, NULL);

  pthread_create(&pTh_par,   NULL, Pares,      NULL);
  pthread_create(&pTh_impar, NULL, Impares,    NULL);
  pthread_create(&pTh_cons,  NULL, Consumidor, NULL);

  pthread_join(pTh_par,   NULL);
  pthread_join(pTh_impar, NULL);
  pthread_join(pTh_cons,  NULL);

  pthread_cond_destroy(&cvImpares); pthread_cond_destroy(&cvPares); pthread_cond_destroy(&cvConsumidor);
  pthread_mutex_destroy(&m);

  exit(0);
}
