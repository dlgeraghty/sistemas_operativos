#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#define MAXLON 1000

//modificar 18 para que haya varios hilos:

//vector de hilos:

void * cuenta( void * nombre){
	printf("hola, soy un hilo fino :)\n");
	int pos, leidos;
	int *cont = (int *)malloc(sizeof(int));
	*cont = 0;
	char cadena[MAXLON];
	int fd;
	fd = open(nombre, O_RDONLY);
	while((leidos = read(fd, cadena, MAXLON)) != 0)
		for(pos = 0; pos < leidos; pos++)
			if((cadena[pos] == 'a') || (cadena[pos] == 'A'))
				(*cont)++;
	close(fd);
	return cont;
}

int main(int argc, char * argv[]){
	if(argc < 2){
		printf("Indica el nombre de al menos un fichero\n");
		exit(0);
	}

	int num_hilos = argc-1;
	pthread_t t[num_hilos];
	int *result;
	for(int i = 0; i < num_hilos; i++)
		pthread_create(&t[i], NULL, cuenta, argv[i+1]);

	for(int i = 0; i < num_hilos; i++){
		pthread_join(t[i], (void*)&result);
		printf("Fichero: %s: %d caracteres son 'a' o 'A'\n", argv[i+1], *result);
	}

	return 0;
}

