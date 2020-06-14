#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace std;

namespace utils {
	vector<int> loadNums(const string& filename) {
		cout << "im here\n";

		vector<int> nums;
		int size;
		ifstream file(filename);
		if (!file) {
			cerr << "Error opening file: " << filename << ".\n";
			throw runtime_error("Could not open the file");
		}
		file >> size;
		nums.resize(size+1);
		for (int i = 0; i < size; i++) {
			file >> nums[i];

		}
		nums[size + 1] = INT_MAX;
		file.close();
		cout << "im here\n";

		return nums;
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
}