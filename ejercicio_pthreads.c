// Producto vectorial.
// Francisco D. Igual.

#include <stdio.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define N 10

#define IMPRIME 1

// Vectores de entrada para operar.
double * vector1;
double * vector2;

// Vector para resultado de producto secuencial.
double * vector3;

// Vector para resultado de producto paralelo.
double * vector4;

// Vector de hilos.
pthread_t * hilos;

// Tamaño de los vectores.
int tam;

// Numero de hilos.
int num_hilos;

// Vector con valor númerico (1...num_hilos) para identificar orden de hilo.
int * id_hilos;

int check( double * a, double *b, double n )
{
  int i = 0;

  for( i = 0; i < n; i++ )
  {
	  if( a[i] != b[i] )
	  {
		  return 1;
	  }
  }
  return 0;
}

void inicializa_vector( double * a, double n )
{
  int i = 0;

  for( i = 0; i < n; i++ )
  {
	  a[i] = rand() % 10;
  }

}

void imprime_vector( double * a, int n )
{
  int i = 0;

  printf( "[ " );

  for( i = 0; i < n; i++ )
  {
    printf( "%f ", a[i] );
  }

  printf( "]\n" );

}

// Punto de entrada para los hilos.
void * pvec_trozo( void * arg )
{
  // COMPLETAR. Recibe un valor entero entre 0 y num_hilos y realiza la operación
  // sobre los elementos correspondientes del vector.
  int i, chunk;
  int numero_hilo = 0/* COMPLETAR */;

  int ini, fin, num_valores;

  num_valores = tam / num_hilos;

  ini = num_valores * numero_hilo;
  fin = ini + num_valores;

  printf( "Thread %ld activo con id %d. Sumara %d valores, desde %d hasta %d\n", pthread_self(), numero_hilo, num_valores, ini, fin );

  // COMPLETAR. Operación

}


// Operacion paralela.
void pvec_paralelo( double * a, double * b, double * c, int n, int num_hilos )
{
	int i = 0;

	// COMPLETAR. Creamos cada hilo e invocamos a pvec_trozo como punto de entrada
	// Proporcionamos un identificador entre 0 y num_hilos para cada nuevo hilo.
	for( i = 0; i < num_hilos; i++ )
	{
	}

	// COMPLETAR. Esperamos a la finalización de los hilos.
	for( i = 0; i < num_hilos; i++ )
	{
	}
}

// Operacion secuencial.
void pvec_secuencial( double * a, double * b, double * c, int n )
{
  int i = 0;

  for( i = 0; i < n; i++ )
  {
    c[i] = a[i] * b[i];
  }

}

// Programa principal.
int main( int argc, char * argv[] )
{
	int i;

        long unsigned int tic, toc;

	if( argc != 3 )
	{
		printf( "Uso: ./pvect tamanyo num_hilos\n" );
		printf( "tamanyo debe ser multiplo del numero de hilos\n" );
		exit( -1 );
	}

	// Recogemos tamanyo de los vectores (num. de elementos).
	tam = atoi( argv[1] );

	// Recogemos numero de hilos.
	num_hilos = atoi( argv[2] );

	if( tam % num_hilos != 0 )
	{
		printf( "Tamanyo debe ser multiplo del numero de hilos\n" );
		exit( -2 );
	}

	// Vector de hilos.
	hilos = (pthread_t*) malloc( num_hilos * sizeof( pthread_t ) );

	// Vector de identificadores de hilo (0...num_hilos-1)
	id_hilos = (int*) malloc( num_hilos * sizeof( int ) );

	// Alojamos vectores en el heap. Seran compartidos por todos los hilos.
	vector1 = (double *) malloc( tam * sizeof( double ) );
	vector2 = (double *) malloc( tam * sizeof( double ) );
	vector3 = (double *) malloc( tam * sizeof( double ) );
	vector4 = (double *) malloc( tam * sizeof( double ) );

	// Inicializamos los vectores.
	inicializa_vector( vector1, tam );
	inicializa_vector( vector2, tam );
	inicializa_vector( vector3, tam );
	inicializa_vector( vector4, tam );

#if IMPRIME
	printf("Contenido original de los vectores: \n");
	imprime_vector( vector1, tam );
	imprime_vector( vector2, tam );
	imprime_vector( vector3, tam );
	imprime_vector( vector4, tam );
#endif

	printf( "Suma secuencial\n" );
	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC, &start);
	pvec_secuencial( vector1, vector2, vector3, tam );
	clock_gettime(CLOCK_MONOTONIC, &end);
	//printf("tiempo antes: %lu, tiempo despues: %lu\n", start.tv_nsec, end.tv_nsec);
	printf( "Fin suma secuencial. Tiempo (en nanosegundos): %lu\n", (end.tv_nsec-start.tv_nsec));

#if IMPRIME
	imprime_vector( vector3, tam );
#endif

	printf( "Suma paralela\n" );
	clock_gettime(CLOCK_MONOTONIC, &start);
	pvec_paralelo( vector1, vector2, vector4, tam, num_hilos );
	clock_gettime(CLOCK_MONOTONIC, &end);
	printf( "Fin suma secuencial. Tiempo (en nanosegundos): %lu\n", (end.tv_nsec-start.tv_nsec));

#if IMPRIME
	imprime_vector( vector4, tam );
#endif
	if( check( vector3, vector4, tam ) == 0 )
	{
		printf( "Resultado correcto!!\n" );
	}
	else
	{
		printf( "Resultado incorrecto!!\n" );
	}


	// Liberamos memoria en el heap.
	free( vector1 );
	free( vector2 );
	free( vector3 );
	free( vector4 );

}
