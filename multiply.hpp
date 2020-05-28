#include <vector>
#include <iostream>
#include <omp.h>

namespace multiply {
	std::vector<std::vector<double>> multiplyInOneThead(std::vector<std::vector<double>> &a, std::vector<std::vector<double>> &b) {
		int64_t rows1 = a.size();
		int64_t inter21 = b.size();
		int64_t columns2 = b[0].size();
		std::vector<std::vector<double>> result(rows1, std::vector<double>(columns2, 0.0));
		for (int64_t row = 0; row < rows1; row++) {
			for (int64_t column = 0; column < columns2; column++) {
				double sum = 0;
				for (int64_t inter = 0; inter < inter21; inter++) {
					sum += a[row][inter] * b[inter][column];
				}
				result[row][column] = sum;
			}
		}
		return result;
	}

	std::vector<std::vector<double>> multiplyParallelStatic(std::vector<std::vector<double>> &a, std::vector<std::vector<double>> &b, int chunkSize) {
		int64_t rows1 = a.size();
		int64_t inter21 = b.size();
		int64_t columns2 = b[0].size();
		std::vector<std::vector<double>> result(rows1, std::vector<double>(columns2, 0.0));
		if (rows1 == 1 && columns2 == 1) {
			double sum = 0;
#pragma omp parallel for reduction(+:sum) schedule(static, chunkSize)
			for (int inner = 0; inner < a[0].size(); inner++) {
				sum += a[0][inner] * b[inner][0];
			}
			result[0][0] = sum;
		} 
		else if (rows1 < 4) {
#pragma omp parallel for schedule(static,chunkSize) shared(a,b)
			for (int64_t i = 0; i < rows1  * columns2; i++) {
				int64_t column = i  % columns2;
				int64_t row = i /  columns2;
				int sum = 0;
				for (int inner = 0; inner < inter21; inner++) {
					sum += a[row][inner] * b[inner][column];
				}
				result[row][column] = sum;
			}
		}
		else {
#pragma omp parallel for schedule(static,chunkSize) shared(a,b)
			for (int64_t row = 0; row < rows1; row++) {
				for (int64_t column = 0; column < columns2; column++) {
					for (int64_t inter = 0; inter < inter21; inter++) {
						result[row][column] += a[row][inter] * b[inter][column];
					}
				}
			}
		}
		return result;
	}

	std::vector<std::vector<double>> multiplyParallelDynamic(std::vector<std::vector<double>>& a, std::vector<std::vector<double>>& b, int chunkSize) {
		int64_t rows1 = a.size();
		int64_t inter21 = b.size();
		int64_t columns2 = b[0].size();
		std::vector<std::vector<double>> result(rows1, std::vector<double>(columns2, 0.0));
		if (rows1 == 1 && columns2 == 1) {
			double sum = 0;
#pragma omp parallel for reduction(+:sum) schedule(dynamic, chunkSize)
			for (int inner = 0; inner < inter21; inner++) {
				sum += a[0][inner] * b[inner][0];
			}
			result[0][0] = sum;
		}
		else if (rows1 < 4) {
#pragma omp parallel for schedule(dynamic,chunkSize) shared(a,b)
			for (int64_t i = 0; i < rows1 * columns2; i++) {
				int64_t column = i % columns2;
				int64_t row = i / columns2;
				int sum = 0;
				for (int inner = 0; inner < inter21; inner++) {
					sum += a[row][inner] * b[inner][column];
				}
				result[row][column] = sum;
			}
		}
		else {
#pragma omp parallel for schedule(dynamic,chunkSize) shared(a,b)
			for (int64_t row = 0; row < rows1; row++) {
				for (int64_t column = 0; column < columns2; column++) {
					for (int64_t inter = 0; inter < inter21; inter++) {
						result[row][column] += a[row][inter] * b[inter][column];
					}
				}
			}
		}
		return result;
	}

	std::vector<std::vector<double>> multiplyParallelGuided(std::vector<std::vector<double>>& a, std::vector<std::vector<double>>& b, int chunkSize) {
		int64_t rows1 = a.size();
		int64_t inter21 = b.size();
		int64_t columns2 = b[0].size();
		std::vector<std::vector<double>> result(rows1, std::vector<double>(columns2, 0.0));
		if (rows1 == 1 && columns2 == 1) {
			double sum = 0;
#pragma omp parallel for reduction(+:sum) schedule(guided, chunkSize)
			for (int inner = 0; inner < inter21; inner++) {
				sum += a[0][inner] * b[inner][0];
			}
			result[0][0] = sum;
		}
		else if (rows1 < 4) {
#pragma omp parallel for schedule(guided,chunkSize) shared(a,b)
			for (int64_t i = 0; i < rows1 * columns2; i++) {
				int64_t column = i % columns2;
				int64_t row = i / columns2;
				int sum = 0;
				for (int inner = 0; inner < inter21; inner++) {
					sum += a[row][inner] * b[inner][column];
				}
				result[row][column] = sum;
			}
		}
		else {
#pragma omp parallel for schedule(guided,chunkSize) shared(a,b)
			for (int64_t row = 0; row < rows1; row++) {
				for (int64_t column = 0; column < columns2; column++) {
					for (int64_t inter = 0; inter < inter21; inter++) {
						result[row][column] += a[row][inter] * b[inter][column];
					}
				}
			}
		}
		return result;
	}
}