#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

void handle_signal_hijo(int sig){
	printf("venga, ya nos vemos en la otra vida :)\n");
	exit(0);
}

int main(){

	signal(SIGUSR1, handle_signal_hijo);

	printf("hola papa, soy tu hijo y tengo pid %d \n", (int)getpid());
	kill(getppid(), SIGUSR1);

	while(1);

}
