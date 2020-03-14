#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//soure : https://www.youtube.com/watch?v=It0OFCbbTJE&list=PL9IEJIKnBJjFZxuqyJ9JqVYmuFZHr7CFM&index=2&frags=wn

void * myturn(void * arg){
	int *iptr = (int *)malloc(sizeof(int));
	*iptr = 5;
	for(int i = 0; i < 8; i++){
		sleep(1);
		printf("My turn %d %d\n", i, *iptr);
		(*iptr)++;
	}
	return iptr;
}
void  yourturn(){
	for(int i = 0; i < 3; i++){
		sleep(2);
		printf("Your turn %d\n", i);
	}
}

int main(){
	pthread_t newthread;

	int *result;
	pthread_create(&newthread, NULL, myturn, NULL);
	yourturn();
	pthread_join(newthread, (void *)&result);
	printf("threads done: *result=%d\n", *result);
	return 0;
}

