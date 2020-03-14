#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//soure: https://www.youtube.com/watch?v=uA8X5zNOGw8&list=PL9IEJIKnBJjFZxuqyJ9JqVYmuFZHr7CFM (very good xD)

void * myturn(void * arg){
	for(int i = 0; i < 8; i++){
		sleep(1);
		printf("My turn%d\n", i);
	}
	return NULL;
}
void  yourturn(){
	for(int i = 0; i < 3; i++){
		sleep(2);
		printf("Your turn %d\n", i);
	}
}

int main(){
	//if we just try to do this:
	//myturn();yourturn();
	//Obviously only the first message will pop up , and since the loop never ends. the second message wont be displayed
	//Solution for both to show:
	pthread_t newthread;
	pthread_create(&newthread, NULL, myturn, NULL);
	yourturn();
	pthread_join(&newthread, NULL);
	return 0;
}

