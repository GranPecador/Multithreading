#include <fstream>
#include <vector>
#include <iostream>

using namespace std;

namespace utils {
	void createFileOfRandomNums(const string &filename, int64_t rows, int64_t columns) {
		std::ofstream fileout(filename, ios_base::out | ios_base::trunc);
		fileout << rows << " " << columns << endl;
		for (int64_t i = 0; i < rows; i++) {
			for (int64_t j = 0; j < columns; j++) {
				fileout << (double)(rand()) / rand() << " ";
			}
			fileout << endl;
		}
		fileout.close();
	}

	vector<vector<double>> loadMatrix(const string &filename) {
		vector<vector<double>> matrix;
		int64_t rows, columns;
		ifstream file(filename);
		if (!file) {
			cerr << "Error opening file: " << filename << ".\n";
			return matrix;
		}
		file >> rows >> columns;
		if (rows < 1 || columns < 1) {
			cerr << "The number of rows and columns must be greater than 0.\n";
			return matrix;
		}
		matrix.resize(rows);
		for (auto &row : matrix) {
			row.resize(columns);
		}
		for (auto &row : matrix) {
			for (auto &num : row) {
				file >> num;
			}
		}
		file.close();
		return matrix;
	}

	void writeInFileResult(vector<vector<double>> result) {
		int64_t rows = result.size();
		int64_t columns = result[0].size();
		ofstream fileout("result.txt", ios_base::trunc);
		fileout << rows << " " << columns << endl;
		for (int64_t i = 0; i < rows; i++) {
			for (int64_t j = 0; j < columns; j++) {
				fileout << result[i][j] << " ";
			}
			fileout << std::endl;
		}
		fileout.close();
	}

	void writeLogTime(string &str) {
		std::ofstream filelog("Log.txt", std::ios::out | std::ios::app);
		filelog << str;
		filelog.close();
	}

	void printMatrix(std::vector<std::vector<double>> matrix) {
		for (int i = 0; i < matrix.size(); i++) {
			for (int j = 0; j < matrix.size(); j++) {
				std::cout << matrix[i][j] << " ";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
}