#include "mpi.h"
#include "utils.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int compare(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

int main(int argc, char** argv) {

    vector<int> nums;
    int numsSize;
    int nDiv;
    int* counts;      /* ������� ���������� ������� */
    int* displacements; /* �������� ���������� ������� */
    vector<int> arrayLocal;

    /*if (argc == 5) {
        utils::createFileOfNums(argv[1], atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
    } 
    /*else if (argc > 2) {
        cout << "Not all arguments specified!" << endl;
        return (1);
    }*/
   // else 
    {
        string inFile(argv[1]);
        string outFile(argv[2]);
        // Initialize the MPI environment
        MPI_Init(&argc, &argv);

        int size;
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        if (rank == 0) {
            nums = utils::loadNums(inFile);
            numsSize = nums.size()-1;
            arrayLocal.resize(numsSize);

            for (auto& num : nums) {
                printf("%d", num);
            }
        }


        MPI_Bcast(&numsSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

        nDiv = numsSize / size;
        counts = new int[size];
        displacements = new int[size];
        for (int p = 0; p < size; p++) {
            counts[p] = nDiv;
            displacements[p] = p * nDiv;
        }
        counts[size - 1] += numsSize % size;

        MPI_Barrier(MPI_COMM_WORLD);
        double wtime = MPI_Wtime();

        /* ���������� ���������� ��������� �������
     � ��� �������� �� ��������� */
        MPI_Scatterv(&nums, counts, displacements,
            MPI_INT, &arrayLocal,
            counts[rank], MPI_INT,
            0, MPI_COMM_WORLD);

        /* ���������� ��������� ���������� */
        if (size == 1) {
            sort(arrayLocal.begin(), arrayLocal.end());
        } {

        }

        /* ���������� ���� ������ �� ���� ����������� */
        MPI_Gatherv(&arrayLocal, counts[rank],
            MPI_INT, &nums, counts,
            displacements, MPI_INT,
            0, MPI_COMM_WORLD);

        /* ���������� �������
������������� ���������� */
        int index;
        vector<int> array_sorted(numsSize);
        if (rank == 0) {
            cout << "im here\n";
            /* ������ ��� ������������ �����, ��������
               ������� ������ �� ������� ������ */

            for (int i = 0; i < numsSize; i++) {
                cout << i << " : " << nums[i] << endl;
            }
            cout << numsSize << " " << nums.size() << endl;
            nums[numsSize] = INT_MAX;
            for (int i = 0; i < numsSize; i++) {
                index = 0;
                for (int p = 1; p < size; p++) {
                    if (nums[displacements[p]] <
                        nums[displacements[index]])
                        index = p;
                }
                array_sorted[i] =
                    nums[displacements[index]];
                displacements[index]++;
                counts[index]--;
                if (counts[index] == 0)
                    displacements[index] = numsSize;
            }
        }

        /* ��������� ����� ������ */
        MPI_Barrier(MPI_COMM_WORLD);
        wtime = MPI_Wtime() - wtime;

        /* ������� ��������������� ������ */
        if (rank == 0) {
            for (int i = 0; i < numsSize; i++)
                printf("%3i ", array_sorted[i]);
            printf("\n");
            printf("Working time: %.2lf seconds\n",
                wtime);
        }
        MPI_Finalize();
    }
    return 0;
}
