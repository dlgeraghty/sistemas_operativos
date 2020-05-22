#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

//simple program to demonstrate the sending and receiving of messages 
//based on: http://condor.cc.ku.edu/~grobe/docs/intro-MPI-C.shtml

#define send_data_tag 2001
#define max_rows 50

int arr[max_rows];
int arr2[max_rows];

main(int argc, char ** argv){
	srand(time(NULL));

	MPI_Status status;

	int i, ierr, root_process, my_id, num_procs, an_id, to_send, to_receive, avg_rows_per_process, start_row, end_row, num_rows_to_send;

	ierr = MPI_Init(&argc, &argv);

	root_process = 0;

	//set some random int to be sent
	to_send = 20;

	//get the process id:
	ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	//get the number of processes:
	ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

	if(my_id == root_process){

		//calculate the length of the array that each slave will process:
		avg_rows_per_process = max_rows / num_procs;
		
		//fill the array with values:

		for(i = 0; i < max_rows; i++)
			arr[i] = rand() % 10;

		printf("The values of the array are: \n");
		for(i = 0; i < max_rows; i++)
			printf("%d " , arr[i]);
		printf("\n");

		//distribute a message to each child process:

		for(an_id = 1; an_id < num_procs; an_id++){
			//calculate the first and last position of the array to compute for each process:
			start_row = an_id * avg_rows_per_process + 1;
			end_row = (an_id + 1) * avg_rows_per_process;

			if((max_rows - end_row) < avg_rows_per_process)
				end_row = max_rows -1;

			num_rows_to_send = end_row - start_row + 1;

			//send: *data_to_send, n of elements to send, MPI datatype, process ID of destination (int), message tag, communicator
			ierr = MPI_Send(&num_rows_to_send, 1, MPI_INT, an_id, send_data_tag, MPI_COMM_WORLD);
			ierr = MPI_Send(&arr[start_row], num_rows_to_send, MPI_INT, an_id, send_data_tag, MPI_COMM_WORLD);
			
		}
	}else{
		ierr = MPI_Recv(&to_receive, 1, MPI_INT, root_process, send_data_tag, MPI_COMM_WORLD, &status);

		ierr = MPI_Recv(&arr2, to_receive, MPI_INT, root_process, send_data_tag, MPI_COMM_WORLD, &status);

		printf("I received a message: %d and an array: \n" , to_receive);
		for(int x = 0; x < to_receive; x++)
			printf("%d ", arr2[x]);
		printf("\n");
	}
	ierr = MPI_Finalize();

	return 0;
}
