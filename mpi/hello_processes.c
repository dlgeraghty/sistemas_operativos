#include <stdio.h>
#include <mpi.h>

main(int argc, char **argv){

	int ierr, num_procs, my_id;
	ierr = MPI_Init(&argc, &argv);

	//get the process id
	ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id); 
	//get the number of processes
	ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

	printf("Hello world! I'm process %i out of %i processes\n", my_id, num_procs);

	ierr = MPI_Finalize();
}
