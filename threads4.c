#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

#define BIG 1000000000UL
uint32_t counter = 0;

void * count_to_big(void * arg){
	for(uint32_t i = 0; i < BIG; i++){
		counter++;
	}
	return NULL;
}

int main(){
	pthread_t t;
	pthread_create(&t, NULL, count_to_big, NULL);
	count_to_big(NULL);
	pthread_join(t, NULL);
	printf("Done, counter: %u\n" , counter);
}

