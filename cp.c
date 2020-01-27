#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define	buffsize	512

int main(int arg, char *argv[]){

	int fdin, fdout, n;
	char buffer[buffsize];

	if( (fdin = open(argv[1], O_RDONLY)) < 0)
			return -1;
	if( (fdout = creat(argv[2] , 0644)) < 0){
		close(fdin);
		return -2;
	}

	while( (n = read(fdin, buffer, buffsize)) > 0){

		if(write(fdout , buffer, n) < n){
			close(fdin);
			close(fdout);
			return -3;
		}
	}
	if(n == -1)
		return -4;
	close(fdin);
	close(fdout);
	return 0;
}
