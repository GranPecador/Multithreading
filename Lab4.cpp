#include <iostream>
#include <vector>
#include "utils.hpp"
#include "dijkstra.hpp"
#include <omp.h>
#include <windows.h>

using namespace std;

using graph = vector<vector<pair<int, int>>>;

int main(int argc, char* argv[])
{
    if (argc == 5) {
        utils::createFileAdjacencyMatrix(argv[1], atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
    } 
    else if (argc < 6) {
        cout << "Not all arguments specified!" << endl;
        return (1);
    }
    else {
        graph graph;
        vector<int> distances;
        char buf[1000];
        int res = -1;

        string matrixFile = argv[1];
        long startNode = atol(argv[2]);
        string resultFile = argv[3];
        int threadNum = atoi(argv[4]);
        string mode = argv[5];
        if (mode != "static" && mode != "dynamic") {
            mode = "static";
        }
        int chunkSize = argv[6] != nullptr ? atoi(argv[6]) : -1;
        omp_set_dynamic(0);
        omp_set_num_threads(threadNum);

        utils::loadMatrix(matrixFile, graph);
        ULONGLONG startTime = GetTickCount64(), timeMultiply;
        if (mode == "static") {
            distances = dijkstra::dijkstraStatic(graph, startNode, chunkSize);
        }
        else {
            distances = dijkstra::dijkstraDynamic(graph, startNode, chunkSize);
        }
        timeMultiply = GetTickCount64() - startTime;
        res = snprintf(buf, sizeof(buf), "%d threads: %s MODE: %I64d size, spent %I64d milliseconds\n", threadNum,mode, graph.size(),timeMultiply);
        string str = "error!";
        if (res >= 0 && res < sizeof(buf))
            str = buf;
        cout << str << std::endl;
        utils::writeLogTime(str);
        utils::printAnswer(startNode, resultFile, distances);
    }
    return 0;
}