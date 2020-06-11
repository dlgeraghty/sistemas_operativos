#include "mpi.h"
#include <stdio.h>

//source: https://computing.llnl.gov/tutorials/mpi/#Blocking_Message_Passing_Routines
//nearest neighbor exchange in a ring topology

void main(int argc, char * argv[]){
	int numprocs, rank, next, prev, buf[2], tag1=1, tag2=2;

	MPI_Request	reqs[4];
	MPI_Status	stats[4];

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	prev = rank-1;
	next = rank+1;
	if(rank == 0) prev = numprocs -1;
	if(rank == (numprocs -1)) next = 0;

	MPI_Irecv(&buf[0], 1, MPI_INT, prev, tag1, MPI_COMM_WORLD, &reqs[0]);
	MPI_Irecv(&buf[1], 1, MPI_INT, next, tag2, MPI_COMM_WORLD, &reqs[1]);

	MPI_Isend(&rank, 1, MPI_INT, prev, tag2, MPI_COMM_WORLD, &reqs[2]);
	MPI_Isend(&rank, 1, MPI_INT, next, tag1, MPI_COMM_WORLD, &reqs[3]);

	MPI_Waitall(4, reqs, stats);

	MPI_Finalize();
}
