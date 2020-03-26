#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>

static int n_child = 0;

void handler(int sig){
	pid_t pid;
	pid = wait(NULL);
	printf("Pid %d exited\n", pid);
	n_child--;
	printf("n_child [%d]\n", n_child);
	if(n_child == 0){
		printf("hijos, estoy orgulloso de vosotros :) ya puedo descansar en paz\n");
		exit(0);
	}
}

int main(int argc, char * argv[]){

	signal(SIGCHLD,handler);
	printf("Ola :P\n");
	int i, status;
	const char *programas[] = { "arandr", "xournalpp"};
	int N = sizeof(programas) / sizeof(programas[0]);
	pid_t pids[N];
	n_child = N;
	for(i = 0; i < N; i++){
		pid_t pid = fork();
		pids[i] = pid;
		if(pid == 0){
			execlp(programas[i], programas[i], NULL);
		}
	}
	getchar();
	printf("adios :)\n");
	return 0;
}
