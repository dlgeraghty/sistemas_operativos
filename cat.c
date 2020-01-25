#include <stdio.h>
#include<fcntl.h>
#include<unistd.h>

#define BUFFSIZE 512

void main(int arg, char *argv[]){

	int fd = 0;
	int cnt = 0;
	char buff[BUFFSIZE];

	fd = open(argv[1], O_RDONLY);

	while((cnt = read(fd, buff, BUFFSIZE)) > 0){
		write(1, buff, cnt);
	}

	close(fd);




}


