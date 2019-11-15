 
#include <iostream>
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <algorithm> 
using namespace std;

void printData(int* data, int n){
	for(int i=0;i<n;i++){
         	printf("%d ",data[i] );
    }
}

int main(int argc, char *argv[]){

	int size,rank;
	int n,elementsPerProcess;
	int *data,recdata[2],recdata2[2];
	int *temp;
	int root_process;
	MPI_Status status;
	
	MPI_Init(&argc, &argv);
    root_process = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

      if(rank == root_process) {
         printf("please enter the number of numbers to sort: ");
         fflush(stdout);
         scanf("%i", &n);

         elementsPerProcess=n / size;
         data = new int[n];

         for(int i = 0; i < n; i++) {
            data[i] = rand()%20;
         }
         printf("array data is:");
         for(int i=0;i<n;i++){
         	printf("%d ",data[i] );
         }
         printf("\n");
    }

    MPI_Bcast(&elementsPerProcess,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Scatter(data, elementsPerProcess, MPI_INT, &recdata, elementsPerProcess, MPI_INT, 0, MPI_COMM_WORLD);

    printf("%d:received data:",rank);
         for(int i=0;i<elementsPerProcess;i++){
         	printf("%d ",recdata[i] );
         }
    printf("\n");

    //begin the odd-even sort
    int oddrank,evenrank;

    if(rank%2==0){
    	oddrank=rank-1; 
    	evenrank = rank+1;
	}
 	else {
 		oddrank = rank+1;
 		evenrank = rank-1;
	}

/* Set the ranks of the processors at the end of the linear */
if (oddrank == -1 || oddrank == size)
 oddrank = MPI_PROC_NULL;
if (evenrank == -1 || evenrank == size)
 evenrank = MPI_PROC_NULL;


for (int index=0; index<size-1; index++) {


 if (index%2 == 1) /* Odd phase */
 MPI_Sendrecv(recdata, elementsPerProcess, MPI_INT, oddrank, 1, recdata2,
 elementsPerProcess, MPI_INT, oddrank, 1, MPI_COMM_WORLD, &status);
 else /* Even phase */
 MPI_Sendrecv(recdata, elementsPerProcess, MPI_INT, evenrank, 1, recdata2,
 elementsPerProcess, MPI_INT, evenrank, 1, MPI_COMM_WORLD, &status);

 //extract elementsPerProcess after sorting the two
 temp= new int[elementsPerProcess];

 for(int i=0;i<elementsPerProcess;i++){
 	temp[i]=recdata[i];
 }

 if(status.MPI_SOURCE==MPI_PROC_NULL)	continue;

 else if(rank<status.MPI_SOURCE){
 	//store the smaller of the two
 	int i,j,k;
 	for(i=j=k=0;k<elementsPerProcess;k++){
 		if(temp[i]<recdata2[j]) {
 			recdata[k]=temp[i++];
 		}
 		else {
 			recdata[k]=recdata2[j++];
 		}
 	}
 }

 else{
 	//store the larger of the two
 	int i,j,k;
 	for(i=j=k=elementsPerProcess-1;k>=0;k--){
 		if(temp[i]>=recdata2[j]) {
 			recdata[k]=temp[i--];
 		}
 		else {
 			recdata[k]=recdata2[j--];
 		}
 	}
 }//else
 }//for

MPI_Gather(recdata,elementsPerProcess,MPI_INT,data,elementsPerProcess,MPI_INT,0,MPI_COMM_WORLD);

if(rank==root_process){
printf("final sorted data:");
printData(data, size);
    printf("\n");
}

MPI_Finalize();

}