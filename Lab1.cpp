#include <iostream>
#include <windows.h>
#include <omp.h>
#include <vector>
#include "utils.hpp"
#include "multiply.hpp"

void multiMatrixWithOneParallel(double** a, double** b, double** c);
void multiMatrixWithTwoParallel(double** a, double** b, double** c);
void multiMatrixWithThreeParallel(double** a, double** b, double** c);
void multiMatrixWithOneParallel_4threads(double** a, double** b, double** c);
void multiMatrixWithTwoParallel_4threads(double** a, double** b, double** c);
void multiMatrixWithThreeParallel_4threads(double** a, double** b, double** c);
void multiMatrixWithReductionSchedule_4threads(double** a, double** b, double** c);
void multiMatrixWithReduction_4threads(double** a, double** b, double** c);
void multiMatrixWith2Parallel_4threadsAndRed(double** a, double** b, double** c);

ULONGLONG dwStart, time;
int64_t rows1 = 1000;
int64_t columns2 = 1500;
int64_t inter21 = 2000;

using namespace std;

int main(int64_t argc, char* argv[]) {

	if (argc < 3) {
		cout << "Not all arguments specified!" << endl;
		return (1);
	} 
	string file1(argv[1]);
	string file2(argv[2]);
	if (argc == 6) {
		utils::createFileOfRandomNums(file1, atoi(argv[3]), atoi(argv[4]));
		utils::createFileOfRandomNums(file2, atoi(argv[4]), atoi(argv[5]));
	} else {
		int maxThreadNum = 4;
		omp_set_num_threads(maxThreadNum);
		vector<vector<double>> a = utils::loadMatrix(file1);
		//utils::printMatrix(a);
		vector<vector<double>> b = utils::loadMatrix(file2);
		//utils::printMatrix(b);

		int mode = argv[3] != nullptr ? atoi(argv[3]) : 1;
		int chunkSize = argv[4] != nullptr ? atoi(argv[4]) : 1;
		vector<vector<double>> result;
		int64_t rows = a.size();
		int64_t inter = b.size();
		int64_t columns = b[0].size();
		char buf[1000];
		int res = -1;
		std::cout << "Begin multiplay" << std::endl;
		ULONGLONG startTime = GetTickCount64(), timeMultiply;
		switch (mode) {
		case(1): {
			result = multiply::multiplyInOneThead(a, b);
			timeMultiply = GetTickCount64() - startTime;
			res = snprintf(buf, sizeof(buf), "ONE THREAD MODE: %I64dx%I64d on %I64dx%I64d, spent %I64d milliseconds\n", rows, inter, inter, columns, timeMultiply);
			break;
		}
		case(2): {
			result = multiply::multiplyParallelStatic(a, b, chunkSize);
			timeMultiply = GetTickCount64() - startTime;
			res = snprintf(buf, sizeof(buf), "STATIC MODE (chunk size = %d) : %I64dx%I64d on %I64dx%I64d, spent %I64d milliseconds\n", chunkSize, rows, inter, inter, columns, timeMultiply);
			break;
		}
		case(3): {
			result = multiply::multiplyParallelDynamic(a, b, chunkSize);
			timeMultiply = GetTickCount64() - startTime;
			res = snprintf(buf, sizeof(buf), "DYNAMIC MODE (chunk size = %d) : %I64dx%I64d on %I64dx%I64d, spent %I64d milliseconds\n", chunkSize, rows, inter, inter, columns, timeMultiply);
			break;
		}
		case(4): {
			result = multiply::multiplyParallelGuided(a, b, chunkSize);
			timeMultiply = GetTickCount64() - startTime;
			res = snprintf(buf, sizeof(buf), "GUIDED MODE (chunk size = %d) : %I64dx%I64d on %I64dx%I64d, spent %I64d milliseconds\n", chunkSize, rows, inter, inter, columns, timeMultiply);
			break;
		}
		default: break;
		}
		std::string str = "error!";
		if (res >= 0 && res < sizeof(buf))
			str = buf;
		std::cout << str << std::endl;
		utils::writeLogTime(str);
		utils::writeInFileResult(result);
	}
	return 0;
}