#include <vector>
#include <algorithm>
#include <iostream>
#include <random>

using namespace std;

namespace quicksort {

	void findPivotAndSwap(MPI_Comm current_communicator);
	void gatherSortArray(int* array, int numsProcess);
	void moveItemsInOrder(int rank, MPI_Comm communicator);

	enum tags {
		ARRAY,
		SIZE,
		INIT,
		ORDER,
	};

	int splitPosition = 0;
	int order = 0;
	int arrayLocalSize = 0;
	int* arrayLocal = nullptr;

	int compare(const void* x1, const void* x2)   
	{
		return (*(int*)x1 - *(int*)x2);
	}

	void quicksort(int* array, int size) {
		qsort(array, size, sizeof(int), compare);      
	}

	void quicksortMPI(int* array, int arrayIntialSize, int rank, int numsProcess) {
		if (rank == 0) {
			// send initial data
			int partSize = arrayIntialSize / numsProcess;
			for (int process = 1; process < numsProcess; process++) {
				int first = process * partSize;
				int last = (process == numsProcess - 1 ? arrayIntialSize : first + partSize);
				int size = last - first;
				MPI_Send(&size, 1, MPI_INT, process, SIZE, MPI_COMM_WORLD);
				MPI_Send(array + first, size, MPI_INT, process, INIT, MPI_COMM_WORLD);
			}
			// copy initial data
			int first = rank * partSize;
			int last = (0 == numsProcess - 1 ? arrayIntialSize : first + partSize);
			arrayLocalSize = last - first;
			arrayLocal = new int[arrayLocalSize];
			memcpy(arrayLocal, array, arrayLocalSize * sizeof(int));
			/*
			cout << "array_part process = " << rank << " : ";
			for (int i = 0; i < array_size; i++) {
				cout << array_working_part[i] << " ";
			}
			cout << endl << endl;
			*/
			findPivotAndSwap(MPI_COMM_WORLD);
			gatherSortArray(array, numsProcess);
		}
		else {
			// wait initial data
			MPI_Recv(&arrayLocalSize, 1, MPI_INT, 0, SIZE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			arrayLocal = new int[arrayLocalSize];
			MPI_Recv(arrayLocal, arrayLocalSize, MPI_INT, 0, INIT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			/*
			cout << "array_part process = "<<rank<<" : ";
			for (int i = 0; i < array_size; i++) {
				cout << array_working_part[i] << " ";
			}
			cout << endl << endl;
			*/
			findPivotAndSwap(MPI_COMM_WORLD);

			// send result
			MPI_Send(&arrayLocalSize, 1, MPI_INT, 0, SIZE, MPI_COMM_WORLD);
			MPI_Send(&order, 1, MPI_INT, 0, ORDER, MPI_COMM_WORLD);
			MPI_Gatherv(arrayLocal, arrayLocalSize, MPI_INT, nullptr, nullptr, nullptr, MPI_INT, 0, MPI_COMM_WORLD);
		}
	}

	void findPivotAndSwap(MPI_Comm currentCommunicator) {

		int currentSize;
		int currentRank;
		MPI_Comm_rank(currentCommunicator, &currentRank);
		MPI_Comm_size(currentCommunicator, &currentSize);
		bool mainProcess = currentRank == 0;

		cout << "cur_size = " << currentSize << endl;

		if (currentSize == 1) {
			quicksort(arrayLocal, arrayLocalSize);
			return;
		}
		
		int pivot ;
		if (mainProcess) {
			// select pivot
			std::random_device rd;
			std::default_random_engine generator(rd());
			std::uniform_int_distribution<int> distribution(0, arrayLocalSize - 1);
			int randIndex = distribution(generator);
			pivot = arrayLocal[randIndex];
		}
		// broadcast pivot
		MPI_Bcast(&pivot, 1, MPI_INT, 0, currentCommunicator);
		//cout << "pivot = " << pivot << endl;
		// rearrange part
		int i, j;
		for (i = 0, j = arrayLocalSize - 1; i < j;) {
			while (i < j && arrayLocal[i] < pivot) i++;
			while (i < j && arrayLocal[j] >= pivot) j--;
			std::swap(arrayLocal[i], arrayLocal[j]);
			//cout << "swap: " << "i" <<" = "<<i<<" array_working_part[i] = "<< array_working_part[i] << endl;
			//cout << "swap: " << "j" <<" = " << j << " array_working_part[j] = " << array_working_part[j] << endl;

		}
		splitPosition = i;
		//cout << "split_pos = " << split_pos << endl;
		
		moveItemsInOrder(currentRank, currentCommunicator);

		MPI_Comm newCommunicator;
		MPI_Comm_split(currentCommunicator, currentRank & 1, 0, &newCommunicator);

		order = 2 * order + (currentRank & 1);
		cout << "order = " << order << endl;
		
		findPivotAndSwap(newCommunicator);
	}

	void moveItemsInOrder(int rank, MPI_Comm communicator) {
		int neighbourDiff = 1;
		bool high = rank & neighbourDiff;
		int sendSize;
		int neighbour;
		int copyFrom;
		int copyTo;
		int sendFrom;

		cout << "updateArr: " << " rank = " << rank << "   "  << endl;

		if (high) {
			neighbour = rank - neighbourDiff;
			sendSize = splitPosition;
			sendFrom = 0;
			copyFrom = splitPosition;
			copyTo = arrayLocalSize;
		}
		else {
			neighbour = rank + neighbourDiff;
			sendSize = arrayLocalSize - splitPosition;
			sendFrom = splitPosition;
			copyFrom = 0;
			copyTo = splitPosition;
		}
		cout << "neighbour: " << neighbour<<" ; send_size = " << sendSize << " ;  send_from  = "<< sendFrom << " ;  copy_from  = "<< copyFrom<< " ;  copy_to =  " << copyTo << endl;


		MPI_Send(&sendSize, 1, MPI_INT, neighbour, SIZE, communicator);

		int recvSize;
		MPI_Recv(&recvSize, 1, MPI_INT, neighbour, SIZE, communicator, MPI_STATUS_IGNORE);

		MPI_Send(arrayLocal + sendFrom, sendSize, MPI_INT, neighbour, ARRAY, communicator);

		cout << "arrayPart: ";
		for (int i = 0; i < arrayLocalSize; i++) {
			cout << arrayLocal[i] << " ";
		}
		cout << endl;

		int sizeTemp = recvSize + arrayLocalSize - sendSize;
		cout << "sizeTemp = " << sizeTemp << endl;
		int* newArray = new int[sizeTemp];
		int j = 0;
		MPI_Recv(newArray, recvSize, MPI_INT, neighbour, ARRAY, communicator, MPI_STATUS_IGNORE);

		for (int i = copyFrom; i < copyTo; i++) {
			int index = i - copyFrom + recvSize;
			newArray[index] = arrayLocal[i];
		}

		delete[] arrayLocal;
		arrayLocal = newArray;
		arrayLocalSize += recvSize - sendSize;
	}

	void gatherSortArray(int* array, int numsProcess) {

		int size = numsProcess;
		int* counts = new int[size];
		int* displacements = new int[size];
		int* orders = new int[size];
		orders[0] = order;
		counts[0] = arrayLocalSize;
		displacements[0] = 0;

		cout << "0" << "    " << arrayLocalSize << "   " << order << endl;

		for (int process = 1, localSize, processOrder; process < size; process++) {
			MPI_Recv(&localSize, 1, MPI_INT, process, SIZE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&processOrder, 1, MPI_INT, process, ORDER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			counts[process] = localSize;
			orders[process] = processOrder;
			cout << process << "    " << localSize << "   " << processOrder << endl;
		}

		for (int processOrder = 0, offset = 0; processOrder < size; processOrder++) {
			for (int process = 0; process < size; process++) {
				if (orders[process] == processOrder) {
					displacements[process] = offset;
					offset += counts[process];
					break;
				}
			}
		}

		MPI_Gatherv(arrayLocal, arrayLocalSize, MPI_INT, array, counts, displacements, MPI_INT, 0, MPI_COMM_WORLD);
	}
}