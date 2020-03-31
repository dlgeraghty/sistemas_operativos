#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/wait.h>
#include <string.h>
#define K 100

//modificar el ejercicio 20 para obtener el numero de espacios en blanco que hay en cada uno de los ficheros dados como parametros
//Se deben lanzar tantos PROCESOS COMO FICHEROS. cada proceso CREARA los HILOS necesarios, que son los que realizaran la busqueda, atendiendo a la restriccion de que un hilo procesara como maximo K caracteres. todos los hilos y procesos se han de ejecutar de forma concurrente. Cada proceso esperara a que terminen sus hilos.

char nombre[100];
int * posiciones;

void * cuenta( void * args){
	//printf("hola, soy un hilo fino %d:)\n", pos_seek);
	int fd, leidos, pos;
	char cadena[K];

	int i = *((int *) args);
	int *cont = malloc(sizeof(int));
	*cont = 0;
	fd = open(nombre, O_RDONLY);

	lseek(fd, (off_t)i, SEEK_SET);
	if((leidos = read(fd, cadena, K)) != 0)
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

	int num_procesos = argc-1;
	for(int x = 0; x < num_procesos; x++){
		pid_t pid = fork();
		if(pid == 0){
			//estoy en el hijo
			//lanzo los hilos.
			printf("\nevaluando fichero %s\n", argv[x+1]);
			strcpy(nombre,argv[x+1]);
			struct stat sb;
			if(stat(argv[x+1], &sb) == -1) perror("stat");
			int num_hilos = sb.st_size/K + 1;

			posiciones = (int*) malloc(num_hilos* sizeof(int));
			posiciones[0] = 0;
			for(int i = 1; i < num_hilos; i++)
				posiciones[i] = posiciones[i-1] + K;
			printf("Number of bytes of file: %d, max bytes per thread: %d, Number of threads: %d\n", (int)sb.st_size, K, num_hilos);
			pthread_t t[num_hilos];
			int * result;
			int suma = 0;
			for(int i = 0; i < num_hilos; i++)
				pthread_create(&t[i], NULL, cuenta, &posiciones[i]);
			for(int i = 0; i < num_hilos; i++){
				pthread_join(t[i], (void*)&result);
				suma += *result;
			}
			printf("Fichero: %s: %d caracteres son 'a' o 'A'\n", argv[x], suma);
			free(posiciones);
		}
		else
			break;
	}
	wait(NULL);
	return 0;
}

