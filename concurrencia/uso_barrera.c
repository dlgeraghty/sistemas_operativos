#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "barrera.h"

#define THREAD_COUNT 4

barrier_t mybarrier;

void* threadFn(void *id_ptr) {
  int thread_id = *(int*)id_ptr;
  int wait_sec = 1 + rand() % 5;
  printf("thread %d: espero %d segundos .\n", thread_id, wait_sec );
  sleep(wait_sec);
  printf("thread %d: Listo y a la espera en la barrera...\n", thread_id);

  barrier_wait(&mybarrier);

  printf("thread %d: continuando!\n", thread_id);
  return NULL;

}


int main() {
  int i;
  pthread_t ids[THREAD_COUNT];
  int short_ids[THREAD_COUNT];

  srand(time(NULL));
  barrier_init(&mybarrier, NULL, THREAD_COUNT + 1);

  for (i=0; i < THREAD_COUNT; i++) {
    short_ids[i] = i;
    pthread_create(&ids[i], NULL, threadFn, &short_ids[i]);
  }

  printf("main() listo y esperando en la barrera.\n");

  barrier_wait(&mybarrier);

  printf("main() continuando!\n");

  for (i=0; i < THREAD_COUNT; i++) {
    pthread_join(ids[i], NULL);
  }

  barrier_destroy(&mybarrier);

  return 0;

}
