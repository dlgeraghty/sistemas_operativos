#include <stdio.h>
#include <mpi.h>

//approximating PI value with Monte Carlo's algorithm 
//source: https://computing.llnl.gov/tutorials/mpi/
#define points 10000
#define tasks 100

int main(){

	//pseudocode:
	//
	//srand(null)
	//
	//num = points/tasks
	//int circle_count = 0;
	//
	//generate processes:
	//
	//if WORKER:
	//from(int i = 1; i < num; i++){
	//
	//	int x = random(0,1)
	//	int y = random(0,1)
	//
	//	if(inside_circle(x,y))	inside_circle??
	//		circle_count++;
	//
	//	send circle_count to master
	//		
	//if MASTER:
	//	receive RESPONSE from workers		
	//	calculate pi approx
	//}

}
