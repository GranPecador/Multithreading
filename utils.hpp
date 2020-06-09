#include <tuple>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

namespace utils {

	void generateInitX(const string& xsFilename, int rows, double initNum) {
		ofstream xsFile(xsFilename, ios_base::trunc);
		xsFile << rows << endl;
		for (int i = 0; i < rows; i++) {
			xsFile << initNum << endl;
		}
		xsFile.close();
	}

	void generateMatrix(const string& matrixFilename, int rows) {
		vector<vector<double>> matrix(rows, vector<double>((int)(rows+1), 0)); ;

		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < rows + 1; j++) {
				matrix[i][j] = rand();
				matrix[i][i] += matrix[i][j];
			}
			matrix[i][i] = matrix[i][i] + 1;
		}

		ofstream matrixFile(matrixFilename, std::ios_base::trunc);
		matrixFile << rows << " " << rows + 1 << endl;
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < (int)(rows+1); j++) {
				matrixFile << matrix[i][j] << " ";
			}
			matrixFile << endl;
		}
		matrixFile.close();
	}

	tuple<double*, double*, double*, int> load(const string &matrixFilename, const string &xsFilename) {
		int rows, columns, m;
		ifstream matrixFile(matrixFilename);
		ifstream xsFile(xsFilename);
		if (!matrixFile || !xsFile) {
			cerr << "Error opening file: " << matrixFilename <<" or "<< xsFilename << ".\n";
			throw std::runtime_error("Can not open the file");
		}
		matrixFile >> rows >> columns;
		if (rows < 1 || columns < 2) {
			cerr << "The number of rows and columns must be greater than 0.\n";
			throw std::runtime_error("Matrix size is invalid.");
		}
		xsFile >> m;
		if (rows != m) {
			cerr << "The number of rows in the matrix should be equal to the number of rows x.\n";
			throw std::runtime_error("The number of x is invalid.");
		}
		double* a = new double[(long)rows*rows];
		double* b = new double[rows];
		double* xs = new double[rows];

		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < rows; j++) {
				matrixFile >> a[i*rows+j];
			}
			matrixFile >> b[i];
			xsFile >> xs[i];
		}
		matrixFile.close();
		xsFile.close();
		return make_tuple(a,b, xs,rows);
	}

	void printVector(char* title, double xLocal[], int n)
	{
			printf("%s\n", title);
			for (int i = 0; i < n; i++)
				printf("%4.1f ", xLocal[i]);
			printf("\n");
	}

	void printResultInFile(const string& outFile, double* matrix, int n) {
		ofstream fout(outFile);
		fout << n << endl;
		for (int i = 0; i < n; i++) {
			fout << matrix[i] << std::endl;
		}
		fout.close();
	}

	void writeLogTime(string& str) {
		std::ofstream filelog("Log.txt", std::ios::out | std::ios::app);
		filelog << str;
		filelog.close();
	}
}