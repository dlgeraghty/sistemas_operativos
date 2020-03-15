#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>

#define SERVER_PORT 18000	//server port we are connecting to
#define MAXLINE 4096		//just some buffer size
#define SA struct sockaddr	//shorthand to type less xD

void err_n_die(const char *fmt, ...);
char *bin2hex(const unsigned char * , size_t);

int main(int argc, char **argv){
	int			listenfd, connfd, n;
	struct sockaddr_in	servaddr;
	uint8_t			buff[MAXLINE+1];
	uint8_t			recvline[MAXLINE+1];

	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)	//try to create a new soket
		err_n_die("socket error");

	//setup the address: (we are listening on)
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family	= AF_INET;
	servaddr.sin_addr.s_addr= htonl(INADDR_ANY);
	servaddr.sin_port	= htons(SERVER_PORT);

	//bind & listen:
	if((bind(listenfd, (SA*) &servaddr, sizeof(servaddr))) < 0)
		err_n_die("bind error");

	if((listen(listenfd, 10)) < 0)
		err_n_die("listen error");

	for( ; ; ){
		struct sockaddr_in addr;
		socklen_t addr_len;
		char client_address[MAXLINE+1];

		printf("waining for a connection on port %d\n", SERVER_PORT);
		fflush(stdout);
		connfd = accept(listenfd, (SA*) &addr, &addr_len);

		inet_ntop(AF_INET, &addr, client_address, MAXLINE);
		printf("Client connection: %s\n", client_address);

		memset(recvline, 0, MAXLINE);
		while((n = read(connfd, recvline, MAXLINE-1)) > 0){
			fprintf(stdout, "\n%s\n\n%s", bin2hex(recvline, n), recvline);
			if(recvline[n-1] == '\n') break;
			memset(recvline, 0, MAXLINE);
		}
		if(n < 0)
			err_n_die("read error");

		//send a response:
		snprintf((char *)buff, sizeof(buff), "HTTP/1.0 200 OK\r\n\r\nHello");
		write(connfd, (char *)buff, strlen((char *)buff));
		close(connfd);
	}
	return 0;
}

char *bin2hex(const unsigned char * input, size_t len){
	char *result;
	char *hexists = "0123456789ABCDEF";

	if(input == NULL || len <= 0)
		return 0;

	int resultlength = (len * 3) + 1;

	result = malloc(resultlength);
	bzero(result, resultlength);

	for(int i = 0; i < len; i++){
		result[i*3]	= hexists[input[i] >> 4];
		result[(i*3)+1] = hexists[input[i] & 0x0F];
		result[(i*3)+2] = ' ';
	}
	return result;
}


void err_n_die(const char * fmt, ...){
	int errno_save;
	va_list ap;
	errno_save = errno;
	va_start(ap, fmt);
	vfprintf(stdout, fmt, ap);
	fprintf(stdout, "\n");
	fflush(stdout);

	if(errno_save != 0){
		fprintf(stdout, "(errno = %d) : %s\n", errno_save, strerror(errno_save));
		fprintf(stdout, "\n");
		fflush(stdout);
	}
	va_end(ap);
	exit(1);
}
