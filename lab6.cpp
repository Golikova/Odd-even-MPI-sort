 
#include <iostream>
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <algorithm> 

#define ROOT 0

using namespace std;

MPI_Status status;

void printData(int* data, int n){
	for(int i=0;i<n;i++){
         	cout << data[i] << " ";
    }
    cout << endl;
}

int main(int argc, char *argv[]){

	int size,rank;
	int elementsPerProcess = 1;
	int *data,currentElement[2],nieghbourElement[2];
	
	MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(rank == ROOT) {
         cout << "Будет создан массив из " << size << " случайных чисел " << endl;
         data = new int[size];
         for(int i = 0; i < size; i++) {
            data[i] = rand()%26;
         }
         cout << "Массив для сортировки: ";
         printData(data, size);
    }

    MPI_Bcast(&elementsPerProcess,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Scatter(data, elementsPerProcess, MPI_INT, &currentElement, elementsPerProcess, MPI_INT, 0, MPI_COMM_WORLD);

    cout << "Процесс №" << rank << " получил элемент: " ;
    printData(currentElement, elementsPerProcess);

    int oddNieghbour,evenNieghbour;

    if(rank%2==0){
    	oddNieghbour=rank-1; 
    	evenNieghbour = rank+1;
	}
 	else {
 		oddNieghbour = rank+1;
 		evenNieghbour = rank-1;
	}

	if (oddNieghbour == -1 || oddNieghbour == size)
	 oddNieghbour = MPI_PROC_NULL;
	if (evenNieghbour == -1 || evenNieghbour == size)
	 evenNieghbour = MPI_PROC_NULL;


	for (int index=0; index<size-1; index++) {

	 if (index%2 == 1) /* Odd phase */
	 MPI_Sendrecv(currentElement, elementsPerProcess, MPI_INT, oddNieghbour, 1, nieghbourElement,
	 elementsPerProcess, MPI_INT, oddNieghbour, 1, MPI_COMM_WORLD, &status);
	 else /* Even phase */
	 MPI_Sendrecv(currentElement, elementsPerProcess, MPI_INT, evenNieghbour, 1, nieghbourElement,
	 elementsPerProcess, MPI_INT, evenNieghbour, 1, MPI_COMM_WORLD, &status);

	 if(status.MPI_SOURCE==MPI_PROC_NULL) continue;

	 int *temp;
	 temp= new int[elementsPerProcess];

	 for(int i=0;i<elementsPerProcess;i++){
	 	temp[i]=currentElement[i];
	 }

	 if(rank<status.MPI_SOURCE){
	 	for(int i=0;i<elementsPerProcess;i++){
	 		if(temp[i]<nieghbourElement[i]) {
	 			currentElement[i]=temp[i];
	 		}
	 		else {
	 			currentElement[i]=nieghbourElement[i];
	 		}
	 	}
	 }

	 else{
	 	for(int i=elementsPerProcess-1;i>=0;i--){
	 		if(temp[i]>=nieghbourElement[i]) {
	 			currentElement[i]=temp[i];
	 		}
	 		else {
	 			currentElement[i]=nieghbourElement[i];
	 		}
	 	}
	 }//else
	 }//for

	MPI_Gather(currentElement,elementsPerProcess,MPI_INT,data,elementsPerProcess,MPI_INT,0,MPI_COMM_WORLD);

	if(rank==ROOT){
	cout << "Результат сортировки: ";
	printData(data, size);
	}

	MPI_Finalize();

}