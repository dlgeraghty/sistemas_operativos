#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

pid_t hijo;

void handle_signal(int sig){
	char answer[1];
	printf("Estas listo para morir, hijo? [s/n]\n");
	scanf("%s", answer);
	if(strcmp(answer, "s") == 0){
		kill(hijo, SIGUSR1);
		wait(&hijo);
		printf("hasta luego\n");
	}
}


int main(void){

	signal(SIGUSR1, handle_signal);

	printf("hola hijo, yo soy tu padre y tengo pid = %d\n" , (int)getpid());
	hijo = fork();
	if(hijo == 0){
		if(execlp("./adios_hijo", "adios_hijo", NULL) == -1){
			printf("Error en exec\n");
			exit(1);
		}
	}
	else
		wait(NULL);


	return 0;
}
