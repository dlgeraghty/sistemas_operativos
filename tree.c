#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

//este programa no busca la eficiencia si no la demostracion de conceptos. Se ofrece iuna breve explicacion a continuacion:
//	en la funcion principal, explore_directories, se recorren todos los directorios hacia abajo solo obviamente, pero solo se buscan los directorios, en ningun momento se referncia a los archivo que estos puedan contener.
//	De los arhivos se encarga el progama ls.c, implementado para una paractica anterior y ligeramente modificado para no mostrar el stat entero si no solo el nombre de los ficheros. Esto es un subproceso que lanza el proceso principal.
//	AL separarlo entre dos procesos surge la cuestion de que perdemos "el contador" de la indentacion para imprimirlo y que quede bonito, ya no lo podemos implementar con una variable global, he resuelto este problema con un fifo extremadamente sencillo, que tan solo contiene este numero entero. El programa que busca en los directorios (este) es el productor y el que busca los ficheros es el consumidor. No voy a explicar en estos comentarios lo que es un fifo de forma detalllada, pero si es importante comentar que es bloqueante. Asi onseguimos la sincronizacion ademas de la comunicacion.

const char * fifo = "/tmp/ls_indentation";

int write_to_fifo(int z){
	int fd = open(fifo, O_WRONLY);
	char str[10];
	sprintf(str, "%d", z);
	str[10] = '\0';
	write(fd, str, 10*sizeof(char));
	close(fd);
}

//node function that allocates a new node with the given data and NULL left and right pointers.
static char * explore_directories(char * s , int z){
	DIR * dir = opendir(s);
	struct dirent * entry = {NULL};

	if(dir == NULL){
		perror("directory");
		exit(1);
	}
	while((entry = readdir(dir)) != NULL){
		if((entry->d_type) == DT_DIR){
			//si si es un directorio:
			char name[1000];
			if(strcmp(entry->d_name, "." ) == 0 || strcmp(entry->d_name, "..") == 0 || entry->d_name[0] == '.')
				continue;
			strcpy(name, s);
			char * next_dir = strcat((strcat(name, "/")), entry->d_name);
			printf("\n\033[1;35m%*s%s\n\033[1;33m",z, "" ,entry->d_name);
			//en vez de entrar por un else despues, podemos ejecutar el ls de la practica anterior:
			//uso un fifo para comunicar los dos procesos, en la comunicacion lo que me interesa que viaje es z.
			int pid = fork();
			char * ls[3];
			//printf("d_name:%s, name:%s, nextdir:%s\n", entry->d_name, name, next_dir);
			ls[0] = "ls.out";
			ls[1] = name;
			//printf("[DEBUG] %s\n", name);
			ls[2] = NULL;
			if(pid == 0){
				if(execv(ls[0], ls))perror("exec");
			}
			else{
				write_to_fifo(z);
				wait(NULL);
			}
			//para hacerlo enrevesadamente interesante, se podria hacer que lanzase un exec con el propio programa y nextdir como parametro xD pero podria llegar a haber muchos procesos abiertos
			explore_directories(next_dir, z+4);
		}
	}
	closedir(dir);
}

int main(int argc, char ** argv){

	mkfifo(fifo, 0666);
	if(argc != 2){
		printf("The program requires an argument\n");
		exit(1);
	}

	explore_directories(argv[1], 0);
}
