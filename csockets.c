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

#define SERVER_PORT 80	//server port we are connecting to
#define MAXLINE 4096	//just some buffer size
#define SA struct sockaddr	//shorthand to type less xD

void err_n_die(const char *fmt, ...);

int main(int argc, char **argv){
	int		sockfd, n;
	int		sendbytes;
	struct sockaddr_in servaddr;
	char		sendline[MAXLINE];
	char		recvline[MAXLINE];

	if(argc != 2)	//usage check
		err_n_die("Usage: %s <server address>", argv[0]);

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)	//create the socket AF_INET -> internet; 0 -> TCP
		err_n_die("Error while creating the socket");

	bzero(&servaddr, sizeof(servaddr));	//zero the addres
	servaddr.sin_family = AF_INET;		//AF_INET->internet
	servaddr.sin_port = htons(SERVER_PORT);	//specify the port

	if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)	//convert 1.2.3.4 to [1,2,3,4]
		err_n_die("inet_pton error for %s ", argv[1]);

	if(connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0)	//try to connect
		err_n_die("connection failed");

	sprintf(sendline, "GET / HTTP/1.1\r\n\r\n");	//build the HTTP request to send
	sendbytes = strlen(sendline);

	if(write(sockfd, sendline, sendbytes) != sendbytes)	//"write" the request into the socket
		err_n_die("write error");

	//get the response from the server:
	memset(recvline, 0, MAXLINE);
	while(( n = read(sockfd, recvline, MAXLINE-1)) > 0){
		printf("%s", recvline);
		memset(recvline, 0, MAXLINE);
	}
	if(n < 0)
		err_n_die("read error");

	return 0;
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
