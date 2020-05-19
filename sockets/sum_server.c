#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define	PORT	8080

int main(int argc, char const * argv[]){

	int server_fd, new_socket;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	int * res;
	char buffer[1024];

	printf("pointer address: %p\n", res);

	//create the socket:
	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	//attach socket to port
	if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	//bind socket to port
	if(bind(server_fd, (struct sockaddr * ) & address, sizeof(address)) < 0){
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	//open communication:
	if(listen(server_fd, 3) < 0){
		perror("listen");
		exit(EXIT_FAILURE);
	}

	//stablish connection:
	if((new_socket = accept(server_fd, (struct sockaddr * ) & address, (socklen_t *) & addrlen)) < 0){
		perror("accept");
		exit(EXIT_FAILURE);
	}

	*res = 0;
	printf("Server sending value: %d\n ", *res);
	send(new_socket, res, sizeof(res), 0);
	printf("Received response from client\n");
	return 0;
}
