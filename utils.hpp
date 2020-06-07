#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <random>

using namespace std;

namespace utils {
	using graph = vector<vector<pair<int, int>>>;
	const int INF = INT_MAX;

    void createFileAdjacencyMatrix(string filename, long nodesSize,  int minBorder, int maxBorder) {
        ofstream file;
        random_device randomDevice;
        mt19937 generator(randomDevice());
        uniform_int_distribution<int> distr(minBorder, maxBorder);
        uniform_int_distribution<int> probability(0, 1);

        vector<vector<int>> matrix(nodesSize);
        for (auto& intra : matrix)
            intra.resize(nodesSize);

        for (long i = 0; i < nodesSize; ++i) {
            for (long j = i; j < nodesSize; ++j) {
                if (j == i) {
                    matrix[i][j] = 0;
                    continue;
                }
                auto prob = probability(generator);
                auto path_len = INF;
                if (prob) {
                    path_len = distr(generator);
                }
                matrix[i][j] = matrix[j][i] = path_len;
            }
        }

        file.open(filename);
        if (file) {
            file << nodesSize << std::endl;
            for (const auto& inner : matrix) {
                for (auto val : inner) {
                    if (val == INF) {
                        file << "INF ";
                        continue;
                    }
                    file << val << " ";
                }
                file << endl;
            }
            file.close();
        }
    }

	void loadMatrix(const string filename, graph& graph) {
        long size;
		string temp;
		ifstream file(filename);
        vector <int> matrix;
		if (!file) {
			cerr << "Error opening file: " << filename << ".\n";
			throw runtime_error("Could not open the file");
		}
		file >> size;
		graph.resize(size);
		matrix.resize(size * size);
		for (long i = 0; i < size; i++) {
			for (long j = 0; j < size; j++) {
				int index = i * size + j;
				file >> temp;
				if ("INF" == temp) {
					matrix[index] = INF;
				}
				else {
					matrix[index] = stoi(temp);
				}
			}
		}
		for (long i = 0; i < size; i++) {
			for (long j = 0; j < size; j++) {
				long index = i * size + j;
				if (matrix[index] != INF) {
					graph[i].push_back(make_pair(j, matrix[index]));
				}
			}
		}
	}

    void printVector(const std::vector<int>& distances) {
        for (long i = 0; i < distances.size(); i++) {
            auto dist = distances[i] == INF ? "INF" : to_string(distances[i]);
            cout << "[" << i << "] - " << dist << endl;
        }
    }

    void writeLogTime(string& str) {
        ofstream filelog("Log.txt", ios::out | ios::app);
        filelog << str;
        filelog.close();
    }

    void printAnswer(int root, const string& outFile, const vector<int>& distances) {
        ofstream fout(outFile, ios_base::trunc);
        fout << "From the node " << root << std::endl;
        for (long i = 0; i < distances.size(); i++) {
            auto dist = distances[i] == INF ? "INF" : std::to_string(distances[i]);
            fout << "[" << i << "] = " << dist << std::endl;
        }
    }
}