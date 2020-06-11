#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <mpi.h>

//approximating PI value with Monte Carlo's algorithm 
//source: https://computing.llnl.gov/tutorials/mpi/
#define points 10000000
#define send_data_tag 2001
#define return_data_tag 2002

int inside_circle(double x, double y){
//	printf("Calculating if inside the circle, with args: %10.8f and %10.8f\n", x, y);
	if((sqrt(x) + sqrt(y)) <= 1.0)
		return 0;
	else 
		return 1;
}

int main(int argc, char ** argv){

	MPI_Status status;

	int num, circle_count, ierr, root_process, num_procs, my_id, i; 
	double pi, to_receive, pi_avg, x, y;

	circle_count = 0;

	ierr = MPI_Init(&argc, &argv);

	root_process = 0;

	//get the id of the process
	ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

	srand((unsigned)time(NULL) + my_id * num_procs);


	num = points/num_procs;

	if(my_id != root_process){
		for(i = 1; i < num; i++){

			x = rand()/(double)RAND_MAX;
			y = rand()/(double)RAND_MAX;

			if(inside_circle(x,y))
				circle_count++;
		}

		printf("I got %d points in the circle out of %d points \n", circle_count, num);

		pi = 4.0 * (double)circle_count/(double)num;

		printf("Calculated my aproximation of pi, is: %f\n", pi);

		ierr = MPI_Send(&pi, 1, MPI_DOUBLE, root_process, send_data_tag, MPI_COMM_WORLD);

	}else{

		//receive circle_count
		for(my_id = 1; my_id < num_procs; my_id++)
			ierr = MPI_Recv(&to_receive, 1, MPI_DOUBLE, MPI_ANY_SOURCE, send_data_tag, MPI_COMM_WORLD, &status);

		printf("receiving pi value: %10.8f\n", to_receive);

	}
	ierr = MPI_Finalize();

	return 0;
}
