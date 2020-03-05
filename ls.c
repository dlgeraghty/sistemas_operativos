#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

const char * fifo = "/tmp/ls_indentation";

int read_from_fifo(){
	int x;
	char str[100];
	int fd = open(fifo, O_RDONLY);
	if(fd == -1) perror("open:");
	read(fd, str, 100);
	close(fd);
	//string to int:
	sscanf(str, "%d", &x);
	//printf("contenido del fifo: %s\n", str);
	return x;
}

int main(int argc, int *argv[])
{

	int z = read_from_fifo();
    struct dirent **namelist;
    int n;

    //printf("[DEBUG][directorio:] %s\n" , argv[1]);
    printf("%*s", z+4, "");
    if(argc == 2){
   n = scandir(argv[1], &namelist, NULL, alphasort);
    if (n < 0)
        perror("scandir");
    else {
        for (int i = 0; i < n; i++) {
		printf("\033[1;31m %s \033[0m", namelist[i]->d_name);
	    free(namelist[i]);
        }
        free(namelist);
    }
    }
    else{
    	printf("wrong arguments\n");
    }
}
