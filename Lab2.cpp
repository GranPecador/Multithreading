#include <iostream>
#include <tuple>
#include <vector>
#include "utils.hpp"
#include "Jacobi.hpp"
#include "mpi.h"


using namespace std;

int main(int argc, char* argv[])
{


    if (argc == 3) {
        utils::generateMatrix(argv[1], atoi(argv[2]));
    } 
    else if (argc == 4) {
        utils::generateInitX(argv[1], atoi(argv[2]), atof(argv[3]));
    }
    else if (argc != 5) {
        cout << "Not all arguments specified!" << endl;
        return (1);
    }
    else {
        double wtime;
        int n, nBar;
        int* countsALocal, * countsBLocal;      /* размеры фрагментов массива */
        int* displacementsALocal, * displacementsBLocal; /* смещения фрагментов массива */
        double* aLocal, *bLocal;
        double* a = nullptr, *b = nullptr;
        double* xs = nullptr;

        string matrixFile(argv[1]);
        string xInputFile(argv[2]);
        double e(atof(argv[3]));
        string xOutputFile(argv[4]);


        MPI_Init(&argc, &argv);
        int rank, numprocs;
        MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        if (numprocs <= 1) {
            throw "Need more than 1 process for completejob.";
        }

        if (rank == 0) {
            tie(a,b, xs, n) = utils::load(matrixFile, xInputFile);
        }
        
        MPI_Barrier(MPI_COMM_WORLD);
        wtime = MPI_Wtime();

        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (rank != 0) {
            xs = new double[n];
        }
        MPI_Bcast(xs, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        
        nBar = n / numprocs;
        countsALocal = new int[numprocs];
        displacementsALocal = new int[numprocs];
        for (int p = 0; p < numprocs; p++) {
            countsALocal[p] = nBar *n;
            displacementsALocal[p] = p * nBar*n;
        }
        countsALocal[numprocs - 1] += (n % numprocs)*n;
        aLocal = new double[countsALocal[rank]];
        countsBLocal = new int[numprocs];
        displacementsBLocal = new int[numprocs];
        for (int p = 0; p < numprocs; p++) {
            countsBLocal[p] = nBar;
            displacementsBLocal[p] = p * nBar;
        }
        countsBLocal[numprocs - 1] += (n % numprocs);
        bLocal = new double[countsBLocal[rank]];
        MPI_Scatterv(a, countsALocal, displacementsALocal, MPI_DOUBLE,
            aLocal, countsALocal[rank], MPI_DOUBLE, 0, MPI_COMM_WORLD);

        MPI_Scatterv(b, countsBLocal, displacementsBLocal, MPI_DOUBLE,
            bLocal, countsBLocal[rank], MPI_DOUBLE, 0, MPI_COMM_WORLD);

        jacobi::jacobi(n, countsBLocal[rank], aLocal,bLocal, xs, e, rank, numprocs, countsBLocal, displacementsBLocal);

        double* result = new double[n];
        MPI_Gatherv(xs, countsBLocal[rank], MPI_DOUBLE, result,countsBLocal,displacementsBLocal, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        
        MPI_Barrier(MPI_COMM_WORLD);
        wtime = MPI_Wtime() - wtime;
        if (rank == 0) {
            //utils::printVector((char*)"The solution is:", result, n);
            char buf[1000];
            int res = -1;
            res = snprintf(buf, sizeof(buf), "Number of processes = %d, matrix A: order of system = %d, spent %f\n", numprocs, n, wtime);
            string str = "error!";
            if (res >= 0 && res < sizeof(buf))
                str = buf;
            std::cout << str << std::endl;
            utils::writeLogTime(str);
            utils::printResultInFile(xOutputFile, result, n);

        }
        MPI_Finalize();
    }
    return 0;
}