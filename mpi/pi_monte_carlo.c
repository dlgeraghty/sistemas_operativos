#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

//approximating PI value with Monte Carlo's algorithm 
//source: https://computing.llnl.gov/tutorials/mpi/
#define points 10000

int inside_circle(int x, int y){
	return 0;
}

int main(int *argc, char ** argv){

	int num, circle_count, ierr, root_process, num_procs, my_id, i, x, y; 

	srand(time(NULL));
	circle_count = 0;

	ierr = MPI_Init(&argc, &argv);

	root_process = 0;

	//get the id of the process
	ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

	num = points/num_procs;

	if(my_id != root_process)
		for(i = 1; i < num; i++){

			x = random();
			y = random();

			if(inside_circle(x,y))
				circle_count++;
			//send circle_count to master;
		}
	else{

		//receive circle_count
	
	}
}
