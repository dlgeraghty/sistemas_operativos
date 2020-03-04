#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <dirent.h>
#include <signal.h>

char p[100];

void prompt(){
	char cwd[1024];
	char * user;
	char hostname[50];
	gethostname(hostname, 50);
	user = getenv("USERNAME");
	getcwd(cwd, sizeof(cwd));
	printf("\033[1;31m[\033[1;33m%s\033[1;36m@\033[1;32m%s~\033[1;35m%s\033[1;31m]\033[0m$ ", user,hostname, cwd);
}

const char * search_path(char * s){
	struct dirent * de;
	DIR *dr = opendir("/bin");
	if(dr == NULL) perror("error opening /bin directory");
	else{
		while((de = readdir(dr)) != NULL)
			if(!strcmp(de->d_name,s)){
				closedir(dr);
				strcpy(p, "/bin/");
				strcat(p, s);
				return p;
			}
	}
	return NULL;
}

int main(int argc, char * argv[], char * envp[]){
	char *tokens[100]={NULL};
	prompt();
	char buf[100];
	char in[100];
	while(fgets(buf, 100, stdin)){
		sscanf(buf, "%[^\n]", in);
		char * token = strtok(in, " ");
		int i = 0;
		while(token != NULL){
			//printf("%s\n",token);
			tokens[i] = token;
			token = strtok(NULL, " ");
			i++;
		}
		char * path_to_program = search_path(tokens[0]);
		if(path_to_program != NULL) tokens[0] = path_to_program;
		//for(int x = 0; x < i; x++)printf("%d\t%s\n", x,tokens[x]);
		tokens[i] = NULL;
		int pid = fork();
		if(pid == 0){
			if(execv(tokens[0], tokens))perror("exec");
		}else{
			wait(NULL);
			prompt();
		}
}
	return 0;
}
