#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

//lanzar a ejecucion dos programas simultaneamente manteniendo el padre vivo:

int main(int argc, char * argv[]){

	printf("Ola :P\n");
	int i, status;
	const char *programas[] = { "brave", "xournalpp"};
	int N = sizeof(programas) / sizeof(programas[0]);
	pid_t pids[N];
	for(i = 0; i < N; i++){
		pid_t pid = fork();
		pids[i] = pid;
		if(pid == 0)
			execlp(programas[i], programas[i], NULL);
	}

	for(int i = 0; i < N; i++){
		pid_t cpid = waitpid(pids[i], &status, 0);
		printf("the %s process finished\n", programas[i]);
	}

	printf("adios :)\n");
	return 0;
}
