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
	
			findPivotAndSwap(MPI_COMM_WORLD);
			gatherSortArray(array, numsProcess);
		}
		else {
			// recive initial data
			MPI_Recv(&arrayLocalSize, 1, MPI_INT, 0, SIZE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			arrayLocal = new int[arrayLocalSize];
			MPI_Recv(arrayLocal, arrayLocalSize, MPI_INT, 0, INIT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

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
		MPI_Bcast(&pivot, 1, MPI_INT, 0, currentCommunicator);
		// rearrange
		int i, j;
		for (i = 0, j = arrayLocalSize - 1; i < j;) {
			while (i < j && arrayLocal[i] < pivot) i++;
			while (i < j && arrayLocal[j] >= pivot) j--;
			std::swap(arrayLocal[i], arrayLocal[j]);
		}
		splitPosition = i;
		
		moveItemsInOrder(currentRank, currentCommunicator);

		MPI_Comm newCommunicator;
		MPI_Comm_split(currentCommunicator, currentRank & 1, 0, &newCommunicator);

		order = 2 * order + (currentRank & 1);
		
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

		int recvSize;
		MPI_Sendrecv(&sendSize, 1, MPI_INT, neighbour, SIZE, &recvSize, 1, MPI_INT, neighbour, SIZE, communicator, MPI_STATUS_IGNORE);
		
		int sizeTemp = recvSize + arrayLocalSize - sendSize;
		int* newArray = new int[sizeTemp];
		MPI_Sendrecv(arrayLocal + sendFrom, sendSize, MPI_INT, neighbour, ARRAY, newArray, recvSize, MPI_INT, neighbour, ARRAY, communicator, MPI_STATUS_IGNORE);

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

		for (int process = 1, localSize, processOrder; process < size; process++) {
			MPI_Recv(&localSize, 1, MPI_INT, process, SIZE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&processOrder, 1, MPI_INT, process, ORDER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			counts[process] = localSize;
			orders[process] = processOrder;
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