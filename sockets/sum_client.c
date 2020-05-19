#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define	PORT	8080

int main(int argc, char const * argv[]){
	int sock = 0, valread;
	struct sockaddr_in serv_addr;
	char * hello = "Hello from client";
	char buffer[1024] = {0};
	char * addr = "192.168.0.24";

	//the address to connect to is a parameter
	

	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("\n Socket creation error \n");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	if(inet_pton(AF_INET, addr, &serv_addr.sin_addr) <= 0){
	       printf("Invalid address / Adress not suppported \n");
	       return -1;
	}

	if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		printf("\n Connection failed \n");
		return -1;
	}

	valread = read( sock, buffer, 1024);
	int * res;
	sscanf(buffer, "%d", res);

	printf("received message from server: %p\n", res);
	return 0;
}
      	       
