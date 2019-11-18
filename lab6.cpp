 
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
	int n;
	int elementsPerProcess;
	int *data,currentElement[100],nieghbourElement[100];
	
	MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(rank == ROOT) {
    	 cout << "Введите размер массива: ";
    	 cin >> n;
         cout << endl;
         elementsPerProcess = n/size;
         data = new int[n];
         for(int i = 0; i < n; i++) {
            data[i] = rand()%100;
         }
         cout << "Массив для сортировки: ";
         printData(data, n);
    }

    MPI_Bcast(&elementsPerProcess,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Scatter(data, elementsPerProcess, MPI_INT, &currentElement, elementsPerProcess, MPI_INT, 0, MPI_COMM_WORLD);

    cout << "Процесс №" << rank << " получил элемент: " ;
    printData(currentElement, elementsPerProcess);

    sort(currentElement,currentElement+elementsPerProcess);

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
	 	//берем меньший
	 	int i,j,k;
	 	for(i=j=k=0;k<elementsPerProcess;k++){
	 		if(temp[i]<nieghbourElement[j])
	 			currentElement[k]=temp[i++];
	 		else
	 			currentElement[k]=nieghbourElement[j++];
	 	}
	 }
	 else{
	 	//берем больший
	 	int i,j,k;
	 	for(i=j=k=elementsPerProcess-1;k>=0;k--){
	 		if(temp[i]>=nieghbourElement[j])
	 			currentElement[k]=temp[i--];
	 		else
	 			currentElement[k]=nieghbourElement[j--];
	 	}
	 }//else

	 cout << "Процесс №" << rank << " обновил свои данные : " ;
	 printData(currentElement, elementsPerProcess);

	 }//for

	MPI_Gather(currentElement,elementsPerProcess,MPI_INT,data,elementsPerProcess,MPI_INT,0,MPI_COMM_WORLD);

	if(rank==ROOT){
	cout << "Результат сортировки: ";
	printData(data, n);
	}

	MPI_Finalize();

}