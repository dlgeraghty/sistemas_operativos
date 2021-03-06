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
#include <pthread.h>

//source tutorial: (must see): https://www.youtube.com/watch?v=Pg_4Jz8ZIH4

#define SERVER_PORT 18000	//server port we are connecting to
#define MAXLINE 4096		//just some buffer size
#define SA struct sockaddr	//shorthand to type less xD
#define SERVER_BACKLOG	100	//number of requests the server will stack till it starts rejecting
#define THREAD_POOL_SIZE 20	//max number of threads

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;

struct node {
	struct node * next;
	int * client_socket;
};
typedef struct node node_t;

node_t * head = NULL;
node_t * tail = NULL;

pthread_t thread_pool[THREAD_POOL_SIZE];

void err_n_die(const char *fmt, ...);
char *bin2hex(const unsigned char * , size_t);
void * handle_connection(void * client_socket);
void * thread_function(void * args);
void enqueue(int *client_socket);
int *dequeue();

int main(int argc, char **argv){
	int			listenfd, connfd, n, addr_size;
	struct sockaddr_in	servaddr, clientaddr;
	uint8_t			buff[MAXLINE+1];
	uint8_t			recvline[MAXLINE+1];

	//crea los threads:
	for(int i = 0; i < THREAD_POOL_SIZE; i++){
		pthread_create(&thread_pool[i], NULL, thread_function, NULL);
	}

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

	if((listen(listenfd, SERVER_BACKLOG)) < 0)
		err_n_die("listen error");

	for( ; ; ){

		printf("waining for a connection on port %d\n", SERVER_PORT);
		addr_size = sizeof(struct sockaddr_in);
		connfd = accept(listenfd, (SA*) &clientaddr, (socklen_t *)&addr_size);

		int * pclient = malloc(sizeof(int));
		*pclient = connfd;
		//thread-safe:
		pthread_mutex_lock(&mutex);
		enqueue(pclient);
		pthread_cond_signal(&condition_var);
		pthread_mutex_unlock(&mutex);

	}
	return 0;
}

void * thread_function(void * args){
	while(1){			//todo:add some sort of termination to this loop
		int * pclient;
		pthread_mutex_lock(&mutex);
		if((pclient = dequeue()) == NULL){
			pthread_cond_wait(&condition_var, &mutex);
			pclient = dequeue();
		}
		pthread_mutex_unlock(&mutex);
		if(pclient != NULL){
			handle_connection(pclient);
		}
	}
}

void * handle_connection(void * p_client_socket){
	int client_socket = *((int *)p_client_socket);
	free(p_client_socket);
	char buffer[MAXLINE];
	size_t bytes_read;
	int msg_size = 0;
	char actualpath[MAXLINE+1];

	while((bytes_read = read(client_socket, buffer+msg_size, sizeof(buffer)-msg_size-1)) > 0){
		msg_size += bytes_read;
		if(msg_size > MAXLINE-1 || buffer[msg_size-1] == '\n') break;
	}
	if(bytes_read < 0) err_n_die("recv error");
	buffer[msg_size-1] = 0;

	printf("REQUEST: %s\n", buffer);
	fflush(stdout);

	if(realpath(buffer, actualpath) == NULL){
		printf("ERROR: bad path: %s\n" , buffer);
		close(client_socket);
		return NULL;
	}

	FILE *fp = fopen(actualpath, "r");
	if(fp == NULL){
		printf("ERROR(open): %s\n", buffer);
		close(client_socket);
		return NULL;
	}

	while((bytes_read = fread(buffer, 1, MAXLINE, fp)) > 0){
		printf("sending %zu bytes\n" , bytes_read);
		write(client_socket, buffer, bytes_read);
	}
	close(client_socket);
	fclose(fp);
	printf("closing connection");

	return NULL;
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

void enqueue(int *client_socket){
	node_t *newnode = malloc(sizeof(node_t));
	newnode->client_socket = client_socket;
	newnode->next = NULL;
	if(tail == NULL){
		head = newnode;
	} else {
		tail->next = newnode;
	}
	tail = newnode;
}

int *dequeue(){
	if(head == NULL){
		return NULL;
	} else {
		int * result = head->client_socket;
		node_t *temp = head;
		head = head->next;
		if(head == NULL) tail = NULL;
		free(temp);
		return result;
	}
}
