#include <stdio.h>
#include <mpi.h>

//simple program to demonstrate the sending and receiving of messages 
//based on: http://condor.cc.ku.edu/~grobe/docs/intro-MPI-C.shtml

#define send_data_tag 2001

main(int argc, char ** argv){

	MPI_Status status;

	int ierr, root_process, my_id, num_procs, an_id, to_send, to_receive;

	ierr = MPI_Init(&argc, &argv);

	root_process = 0;

	//set some random int to be sent
	to_send = 20;

	//get the process id:
	ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	//get the number of processes:
	ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

	if(my_id == root_process){

		//distribute a message to each child process:

		for(an_id = 1; an_id < num_procs; an_id++){

			//send: *data_to_send, n of elements to send, MPI datatype, process ID of destination (int), message tag, communicator
			ierr = MPI_Send(&to_send, 1, MPI_INT, an_id, send_data_tag, MPI_COMM_WORLD);
		}
	}else{
		ierr = MPI_Recv(&to_receive, 1, MPI_INT, root_process, send_data_tag, MPI_COMM_WORLD, &status);

		printf("I received a message: %d \n" , to_receive);
	}
	ierr = MPI_Finalize();

	return 0;
}
