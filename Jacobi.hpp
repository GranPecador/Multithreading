
#define Swap(x,y) {double* temp; temp = x; x = y; y = temp; }

namespace jacobi {
	double distance(double* x, double* y, int n)
	{
		double sum = 0.0;
		for (int i = 0; i < n; i++) {
			sum = sum + (x[i] - y[i]) * (x[i] - y[i]);
		}
		cout << "distancies: " << sqrt(sum) << endl;

		return sqrt(sum);
	}

	void jacobi(int n, int countRows, int*aLocal, int*bLocal, double*xs,double e,int rank, int numprocs, int* countBLocal, int* displacementsBLocal) {
		double* xOld = new double[n];
		double* xNew = new double[n];
		int nBar = n / numprocs;
		MPI_Allgatherv(xs, countRows, MPI_DOUBLE, xNew, countBLocal, displacementsBLocal, MPI_DOUBLE, MPI_COMM_WORLD);
		
		do {
			cout << "\n Old" << endl;

			for (int ii = 0; ii < n; ii++) {
				printf(": %f :", xOld[ii]);
			}
			cout << "\n New" << endl;

			for (int ii = 0; ii < n; ii++) {
				printf(": %f :", xNew[ii]);
			}
			cout << "\n" << endl;

			Swap(xOld, xNew);
			for (int iLocal = 0; iLocal < countRows; iLocal++) {
				xs[iLocal] = bLocal[iLocal];
				for (int j = 0; j < n; j++) {
					if (j != iLocal + displacementsBLocal[rank]) {
						xs[iLocal] -= aLocal[iLocal * n + j] * xOld[j];
					}
				}
				xs[iLocal] = xs[iLocal] / aLocal[iLocal * n + iLocal + displacementsBLocal[rank]];
				printf("delitel: %d \n", aLocal[iLocal * n + iLocal + displacementsBLocal[rank]]);
			}
			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Allgatherv(xs, countRows, MPI_DOUBLE, xNew, countBLocal, displacementsBLocal, MPI_DOUBLE, MPI_COMM_WORLD);
			
		} while (distance(xNew, xOld,n) >= e);
	}

	
}