#include "mpi.h"
#include "utils.hpp"
#include "quicksort.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <tuple>


using namespace std;

int compare(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

int main(int argc, char** argv) {

    int* nums = nullptr;
    int numsSize = 0;

    if (argc == 5) {
        utils::createFileOfNums(argv[1], atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
    } 
    else if (argc != 3) {
        cout << "Not all arguments specified!" << endl;
        return (1);
    }
    else 
    {
        string inFile(argv[1]);
        string outFile(argv[2]);
        MPI_Init(&argc, &argv);

        int size;
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        if (rank == 0) {
            tie(nums, numsSize) = utils::loadNums(inFile);
        }
        MPI_Bcast(&numsSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
        double wtime = MPI_Wtime();

        if (size == 1) {
            quicksort::quicksort(nums, numsSize);
        } 
        else {
            quicksort::quicksortMPI(nums, numsSize, rank, size);
        }
         
        MPI_Barrier(MPI_COMM_WORLD);
        wtime = MPI_Wtime() - wtime;

        if (rank == 0) {
            char buf[1000];
            int res = -1;
            res = snprintf(buf, sizeof(buf), "%d process/es: %d size, spent %f time\n", size, numsSize, wtime);
            string str = "error!";
            if (res >= 0 && res < sizeof(buf))
                str = buf;
            cout << str << std::endl;
            utils::writeLogTime(str);
            utils::printAnswer(nums, numsSize, outFile);
        }
        MPI_Finalize();
    }
    return 0;
}
