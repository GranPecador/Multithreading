#include <iostream>
#include <fstream>
#include <windows.h>
#include <omp.h>

void createFileOfRandomNums(char* name, int64_t rows, int64_t columns);
double** readMatrix(char* name);
void multiMatrix(double** a, double** b, double** c);
void multiMatrixWithParallel(double** a, double** b, double** c);
void multiMatrixWithOneParallel(double** a, double** b, double** c);
void multiMatrixWithTwoParallel(double** a, double** b, double** c);
void multiMatrixWithThreeParallel(double** a, double** b, double** c);
void multiMatrixWithOneParallel_4threads(double** a, double** b, double** c);
void multiMatrixWithTwoParallel_4threads(double** a, double** b, double** c);
void multiMatrixWithThreeParallel_4threads(double** a, double** b, double** c);
void printMatrix(double** c, int64_t rows, int64_t columns);
void writeMatrixInFile(char* name, double** result, int64_t rows, int64_t columns);
void writeMatrixInFile(char* name, double* result, int64_t rows, int64_t columns);
double** createResultMatrixEmpty();
double* createResultMatrixFillZero();
void multiMatrixOneCycle(double** a, double** b, double* c);
void multiMatrixOneCycleWithParallel(double** a, double** b, double* c);

ULONGLONG dwStart;
int64_t rows1 = 1000;
int64_t columns2 = 1500;
int64_t inter21 = 2000;

int main(int64_t argc, char* argv[]) {

	double** a = nullptr, ** b = nullptr, ** c;
	double* cc;

	//createFileOfRandomNums((char*)("matrix10002000.txt"), rows1, inter21);
	//createFileOfRandomNums((char*)("matrix15002000.txt"), inter21, columns2);

	// Ввод элементов первой матрицы
	a = readMatrix((char*)"matrix10002000.txt");
	//printMatrix(a, rows1, inter21);
	// Ввод элементов первой матрицы
	b = readMatrix((char*)"matrix15002000.txt");
	//printMatrix(b, inter21, columns2);
	cc = createResultMatrixFillZero();
	multiMatrixOneCycle(a, b, cc);
	writeMatrixInFile((char*)"resultOneCycle.txt", cc, rows1, columns2);
	cc = createResultMatrixFillZero();
	multiMatrixOneCycleWithParallel(a, b, cc);
	writeMatrixInFile((char*)"resultOneCycleParallel.txt", cc, rows1, columns2);

	c = createResultMatrixEmpty();
	multiMatrix(a,b, c);
	writeMatrixInFile((char*)"result.txt", c, rows1, columns2);
	multiMatrixWithParallel(a, b, c);
	writeMatrixInFile((char*)"result0.txt", c, rows1, columns2);
	//multiMatrixWithOneParallel(a, b, c);
	//writeMatrixInFile((char*)"result1.txt", c, rows1, columns2);
	//multiMatrixWithTwoParallel(a, b, c);
	//writeMatrixInFile((char*)"result2.txt", c, rows1, columns2);
	//multiMatrixWithThreeParallel(a, b, c);
	//writeMatrixInFile((char*)"result3.txt", c, rows1, columns2);
	//multiMatrixWithOneParallel_4threads(a, b, c);
	//writeMatrixInFile((char*)"result7.txt", c, rows1, columns2);
	//multiMatrixWithTwoParallel_4threads(a, b, c);
	//writeMatrixInFile((char*)"result8.txt", c, rows1, columns2);
	//multiMatrixWithThreeParallel_4threads(a, b, c);
	//writeMatrixInFile((char*)"result9.txt", c, rows1, columns2);

	return (0);
}

void createFileOfRandomNums(char* name, int64_t rows, int64_t columns) {
	std::ofstream f1(name, std::ios::out | std::ios::app);
	f1 <<rows<<" "<<columns<< std::endl;
	for (int64_t i = 0; i < rows; i++) {
		for (int64_t j = 0; j < columns; j++) {
			f1 << (double)(rand()) / rand() << " ";
		}
		f1 << std::endl;
	}
	f1.close();
}

void writeMatrixInFile(char* name, double** result, int64_t rows, int64_t columns) {
	std::ofstream f1(name, std::ios::out | std::ios::app);
	f1 << rows << " " << columns << std::endl;
	for (int64_t i = 0; i < rows; i++) {
		for (int64_t j = 0; j < columns; j++) {
			f1 << result[i][j] << " ";
		}
		f1 << std::endl;
	}
	f1.close();
}

void writeMatrixInFile(char* name, double* result, int64_t rows, int64_t columns) {
	std::ofstream f1(name, std::ios::out | std::ios::app);
	f1 << rows << " " << columns << std::endl;
	for (int64_t i = 0; i < rows*columns; i++) {
		f1 << result[i] << " ";
		if (i % columns == 9) {
			f1 << std::endl;
		}
	}
	f1.close();
}

double** readMatrix(char* name) {
	std::ifstream file(name);
	int64_t rows, columns;
	double** matrix;
	file >> rows >> columns;
	if (name == "matrix1.txt") {
		rows1 = rows;
		inter21 = columns;
	}
	else {
		inter21 = rows;
		columns2 = columns;
	}
	matrix = new double* [rows]; 
	for (int64_t row = 0; row < rows; row++) {
		matrix[row] = new double [columns]; 
		for (int64_t column = 0; column < columns; column++) {
			file>>matrix[row][column];
		}
	}
	file.close();
	return matrix;
}
double** createResultMatrixEmpty() {
	double** result;
	result = new double* [rows1];
	for (int64_t row = 0; row < rows1; row++) {
		result[row] = new double[columns2];
	}
	return result;
}

double* createResultMatrixFillZero() {
	double* result;
	result = new double [rows1* columns2];
	for (int64_t i = 0; i < rows1 * columns2; i++) {
			result[i] = 0;
	}
	return result;
}

void multiMatrixOneCycle(double** a, double** b, double* c) {
	std::cout << "Begin multiplay in one cycle" << std::endl;
	dwStart = GetTickCount64();
	int64_t count = rows1 * inter21 * columns2;
	for (int64_t i = 0; i < count; i++) {
		int64_t temp = i % inter21;
		int64_t column = i / inter21 % columns2;
		int64_t row = i / (inter21 * columns2) ;
		//std::cout << i << " " << row << " " << column << " " << temp  << std::endl;
		c[row*columns2+column] += a[row][temp] * b[temp][column];
	}
	printf_s("For multiply two matrixs: %I64dx%I64d on %I64dx%I64d, spent %I64d milliseconds\n", rows1, inter21, inter21, columns2, (GetTickCount64() - dwStart));
}

void multiMatrixOneCycleWithParallel(double** a, double** b, double* c) {
	std::cout << "Begin multiplay in one cycle with parallel" << std::endl;
	dwStart = GetTickCount64();
	int64_t count = rows1 * inter21 * columns2;
#pragma omp parallel for schedule(runtime) 
		for (int64_t i = 0; i < count; i++) {
			//std::cout << "thread: " << omp_get_thread_num() << std::endl;
			int64_t temp = i % inter21;
			int64_t column = i / inter21 % columns2;
			int64_t row = i / (inter21 * columns2);
			//std::cout << i << " " << row << " " << column << " " << temp  << std::endl;
			double multi = a[row][temp] * b[temp][column];
#pragma omp critical
			c[row * columns2 + column] += multi;
		}
	
	printf_s("For multiply two matrixs: %I64dx%I64d on %I64dx%I64d, spent %I64d milliseconds\n", rows1, inter21, inter21, columns2, (GetTickCount64() - dwStart));
}

void multiMatrix(double** a, double** b, double** c) {
	std::cout << "Begin multiplay" << std::endl;
	dwStart = GetTickCount64();
	for (int64_t row = 0; row < rows1; row++) {
		for (int64_t column = 0; column < columns2; column++) {
			c[row][column] = 0;
			for (int64_t inter = 0; inter < inter21; inter++) {
				c[row][column] += a[row][inter] * b[inter][column];
			}
		}
	}
	printf_s("For multiply two matrixs: %I64dx%I64d on %I64dx%I64d, spent %I64d milliseconds\n", rows1, inter21, inter21, columns2, (GetTickCount64() - dwStart));
}
void multiMatrixWithParallel(double** a, double** b, double** c) {
	std::cout << "Begin multiplay with parallel" << std::endl;
	dwStart = GetTickCount64();
#pragma omp parallel
	{
		for (int64_t row = 0; row < rows1; row++) {
			for (int64_t column = 0; column < columns2; column++) {
				c[row][column] = 0;
				for (int64_t inter = 0; inter < inter21; inter++) {
					c[row][column] += a[row][inter] * b[inter][column];
				}
			}
		}
	}
	printf_s("For multiply two matrixs: %I64dx%I64d on %I64dx%I64d, spent %I64d milliseconds\n", rows1, inter21, inter21, columns2, (GetTickCount64() - dwStart));
}
void multiMatrixWithOneParallel(double** a, double** b, double** c) {
	std::cout << "\nBegin multiplay with OpenMP : One paralleled loop" << std::endl;
	dwStart = GetTickCount64();
#pragma omp parallel
	{
#pragma omp for
		for (int64_t row = 0; row < rows1; row++) {
			for (int64_t column = 0; column < columns2; column++) {
				c[row][column] = 0;
				for (int64_t inter = 0; inter < inter21; inter++) {
					c[row][column] += a[row][inter] * b[inter][column];
				}
			}
		}
	}
	printf_s("For multiply two matrixs: %I64dx%I64d on %I64dx%I64d, spent %I64d milliseconds\n", rows1, inter21, inter21, columns2, (GetTickCount64() - dwStart));
}

void multiMatrixWithTwoParallel(double** a, double** b, double** c) {
	std::cout << "\nBegin multiplay with OpenMP : Two paralleled loops" << std::endl;
	dwStart = GetTickCount64();
#pragma omp parallel
	{
#pragma omp for
		for (int64_t row = 0; row < rows1; row++) {
#pragma omp parallel for
			for (int64_t column = 0; column < columns2; column++) {
				c[row][column] = 0;
				for (int64_t inter = 0; inter < inter21; inter++) {
					c[row][column] += a[row][inter] * b[inter][column];
				}
			}
		}
	}
	printf_s("For multiply two matrixs: %I64dx%I64d on %I64dx%I64d, spent %I64d milliseconds\n", rows1, inter21, inter21, columns2, (GetTickCount64() - dwStart));
}

void multiMatrixWithThreeParallel(double** a, double** b, double** c) {
	
	std::cout << "\nBegin multiplay with OpenMP : Three paralleled loops" << std::endl;
	dwStart = GetTickCount64();
#pragma omp parallel
	{
#pragma omp for
		for (int64_t row = 0; row < rows1; row++) {
#pragma omp parallel for
			for (int64_t column = 0; column < columns2; column++) {
				double temp = 0;
#pragma omp parallel for reduction(+:temp) schedule(static,4) 
				for (int64_t inter = 0; inter < inter21; inter++) {
					temp += a[row][inter] * b[inter][column];
				}
				c[row][column] = temp;
			}
		}
	}
	printf_s("For multiply two matrixs: %I64dx%I64d on %I64dx%I64d, spent %I64d milliseconds\n", rows1, inter21, inter21, columns2, (GetTickCount64() - dwStart));
}

void multiMatrixWithOneParallel_4threads(double** a, double** b, double** c) {
	std::cout << "\nBegin multiplay with OpenMP : One paralleled loop on 4 threads" << std::endl;
	dwStart = GetTickCount64();
#pragma omp parallel num_threads(4)
	{
#pragma omp for
		for (int64_t row = 0; row < rows1; row++) {
			for (int64_t column = 0; column < columns2; column++) {
				c[row][column] = 0;
				for (int64_t inter = 0; inter < inter21; inter++) {
					c[row][column] += a[row][inter] * b[inter][column];
				}
			}
		}
	}
	printf_s("For multiply two matrixs: %I64dx%I64d on %I64dx%I64d, spent %I64d milliseconds\n", rows1, inter21, inter21, columns2, (GetTickCount64() - dwStart));
}

void multiMatrixWithTwoParallel_4threads(double** a, double** b, double** c) {
	std::cout << "\nBegin multiplay with OpenMP : Two paralleled loops on 4 threads" << std::endl;
	dwStart = GetTickCount64();
#pragma omp parallel num_threads(4)
	{
#pragma omp for
		for (int64_t row = 0; row < rows1; row++) {
#pragma omp parallel for
			for (int64_t column = 0; column < columns2; column++) {
				c[row][column] = 0;
				for (int64_t inter = 0; inter < inter21; inter++) {
					c[row][column] += a[row][inter] * b[inter][column];
				}
			}
		}
	}
	printf_s("For multiply two matrixs: %I64dx%I64d on %I64dx%I64d, spent %I64d milliseconds\n", rows1, inter21, inter21, columns2, (GetTickCount64() - dwStart));
}

void multiMatrixWithThreeParallel_4threads(double** a, double** b, double** c) {
	std::cout << "\nBegin multiplay with OpenMP : Three paralleled loops on 4 threads" << std::endl;
	dwStart = GetTickCount64();
#pragma omp parallel num_threads(4)
	{
#pragma omp for
		for (int64_t row = 0; row < rows1; row++) {
#pragma omp parallel for
			for (int64_t column = 0; column < columns2; column++) {
				double temp = 0;
#pragma omp parallel for reduction(+:temp) schedule(static, 4)
				for (int64_t inter = 0; inter < inter21; inter++) {
					temp += a[row][inter] * b[inter][column];
				}
				c[row][column] = temp;
			}
		}
	}
	printf_s("For multiply two matrixs: %I64dx%I64d on %I64dx%I64d, spent %I64d milliseconds\n", rows1, inter21, inter21, columns2, (GetTickCount64() - dwStart));
}

void printMatrix(double** c, int64_t rows, int64_t columns) {
	// Вывод матрицы произведения

	std::cout << "Матрица" << std::endl;
	for (int64_t i = 0; i < rows; i++)
	{
		for (int64_t j = 0; j < columns; j++)
			std::cout << c[i][j] << " ";
		std::cout << std::endl;
	}
}
