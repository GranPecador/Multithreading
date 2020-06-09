#include <iostream>
#include <tuple>
#include <vector>
#include "mpi.h"
#include "utils.hpp"
#include "Jacobi.hpp"


using namespace std;

int main(int argc, char* argv[])
{


    if (argc == 3) {
        utils::generateMatrix(argv[1], atoi(argv[2]));
    } 
    else if (argc == 4) {
        utils::generateInitX(argv[1], atoi(argv[2]), atoi(argv[3]));
    }
    else if (argc != 5) {
        cout << "Not all arguments specified!" << endl;
        return (1);
    }
    else {
        double wtime;
        int n, n_bar;
        int* countsALocal, * countsBLocal;      /* размеры фрагментов массива */
        int* displacementsALocal, * displacementsBLocal; /* смещения фрагментов массива */
        int* aLocal, *bLocal;
        int* a = nullptr, *b = nullptr;
        double* xs = nullptr;
        double* final_coeffs = nullptr;

        string matrix_file(argv[1]);
        string x_input_file(argv[2]);
        double e(atof(argv[3]));
        string xOutputFile(argv[4]);


        MPI_Init(&argc, &argv);
        int rank, numprocs, maxIter = 100;
        MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        std::cout << "rank " << rank << " from " << numprocs << std::endl;

        if (rank == 0) {
            tie(a,b, xs, n) = utils::load(matrix_file, x_input_file);
            
        }
        /* Запускаем таймер */
        MPI_Barrier(MPI_COMM_WORLD);
        wtime = MPI_Wtime();

        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (rank != 0) {
            xs = new double[n];
        }
        MPI_Bcast(xs, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        
        n_bar = n / numprocs;
        countsALocal = new int[numprocs];
        displacementsALocal = new int[numprocs];
        for (int p = 0; p < numprocs; p++) {
            countsALocal[p] = n_bar *n;
            displacementsALocal[p] = p * n_bar*n;
        }
        countsALocal[numprocs - 1] += (n % numprocs)*n;
        aLocal = new int[countsALocal[rank]];
        countsBLocal = new int[numprocs];
        displacementsBLocal = new int[numprocs];
        for (int p = 0; p < numprocs; p++) {
            countsBLocal[p] = n_bar;
            displacementsBLocal[p] = p * n_bar;
        }
        countsBLocal[numprocs - 1] += (n % numprocs);
        bLocal = new int[countsBLocal[rank]];
        MPI_Scatterv(a, countsALocal, displacementsALocal, MPI_INT,
            aLocal, countsALocal[rank], MPI_INT,
            0, MPI_COMM_WORLD);
        for (int i = 0; i < countsALocal[rank]; i++) {
            printf("%d ", aLocal[i]);
        }
        MPI_Scatterv(b, countsBLocal, displacementsBLocal, MPI_INT,
            bLocal, countsBLocal[rank], MPI_INT, 0, MPI_COMM_WORLD);
        cout<<"\n" << endl;
        for (int i = 0; i < countsBLocal[rank]; i++) {
            printf("   %d   ", bLocal[i]);
        }

        jacobi::jacobi(n, countsBLocal[rank], aLocal,bLocal, xs, e, rank, numprocs, countsBLocal, displacementsBLocal);

        double* result = new double[n];
        MPI_Gatherv(xs, countsBLocal[rank], MPI_DOUBLE, result,countsBLocal,displacementsBLocal, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        /* Вычисляем время работы */
        MPI_Barrier(MPI_COMM_WORLD);
        wtime = MPI_Wtime() - wtime;
        if (rank == 0) {
            utils::printVector((char*)"The solution is:", result, n);
            char buf[1000];
            int res = -1;
            res = snprintf(buf, sizeof(buf), "Matrix A: order of system = %d, spent %f milliseconds\n", n, wtime);
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