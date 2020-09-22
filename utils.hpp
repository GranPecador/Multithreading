#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <tuple>

using namespace std;

namespace utils {
	tuple<int*, int> loadNums(const string& filename) {
		int* nums;
		int size;
		ifstream file(filename);
		if (!file) {
			cerr << "Error opening file: " << filename << ".\n";
			MPI_Abort(MPI_COMM_WORLD, 1);
			throw runtime_error("Could not open the file");
		}
		file >> size;
		nums = new int[size];
		for (int i = 0; i < size; i++) {
			file >> nums[i];

		}
		file.close();
		return make_tuple(nums, size);
	}

	void createFileOfNums(const string& filename, int size, int begin, int end) {
		srand((unsigned)time(0));
		ofstream fileout(filename, ios_base::trunc);
		fileout << size<<endl;
		for (int i = 0; i < size; i++) {
			fileout << (rand() % end) + begin << " ";
		}
		fileout.close();
	}

	void writeLogTime(string& str) {
		ofstream filelog("Log.txt", ios::out | ios::app);
		filelog << str;
		filelog.close();
	}

	void printAnswer(int* nums, int numsSize, const string& outFile) {
		ofstream fout(outFile, ios_base::trunc);
		fout << "Size: " << numsSize << std::endl;
		for (int i = 0; i < numsSize; i++)
			fout << nums[i] << " ";
		fout.close();
	}
}