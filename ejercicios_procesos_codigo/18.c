#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#define MAXLON 1000

//hilo que ejecuta una funcion contar:

void * cuenta( void * nombre){
	printf("hola, soy un hilo fino :)\n");
	int pos, cont = 0, leidos;
	char cadena[MAXLON];
	int fd;
	fd = open(nombre, O_RDONLY);

	while((leidos = read(fd, cadena, MAXLON)) != 0)
		for(pos = 0; pos < leidos; pos++)
			if((cadena[pos] == 'a') || (cadena[pos] == 'A'))
				cont++;
	printf("Fichero: %s: %d caracteres son 'a' o 'A'\n", (char*)nombre, cont);
	close(fd);
	return NULL;
}

int main(int argc, char * argv[]){
	if(argc != 2){
		printf("Indica el nombre de un fichero\n");
		exit(0);
	}
	pthread_t hilo;
	pthread_create(&hilo, NULL, cuenta, argv[1]);
	pthread_join(hilo, NULL);
	return 0;
}

