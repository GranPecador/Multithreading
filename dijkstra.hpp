#include <vector>
#include <omp.h>

using namespace std;

namespace dijkstra {
    using graph = vector<vector<pair<int, int>>>;
    const int INF = INT_MAX;
	
	vector<int> dijkstraStatic(graph graph, long startNode, long chunkSize) {

        long j, path_len, len, dest, graph_size, shortest;
        long nodes = graph.size();
       
        vector<int> distances(nodes, INF), parents(nodes);
        vector<char> visited(nodes);
        vector<int> thread_min(omp_get_max_threads(), INF);
        vector<long> thread_id(omp_get_max_threads(), nodes + 1);
        distances[startNode] = 0;
        if (chunkSize == -1) {
            chunkSize = nodes / omp_get_max_threads();
        }

        for (long i = 0; i < nodes; ++i) {
            fill(thread_min.begin(), thread_min.end(), INF);
            fill(thread_id.begin(), thread_id.end(), nodes + 1);
#pragma omp parallel for schedule(static, chunkSize) firstprivate(nodes) private(shortest, j)
            for (j = 0; j < nodes; ++j) {
                shortest = thread_id[omp_get_thread_num()];
                if (!visited[j] && (shortest == nodes + 1
                        || distances[j] < distances[shortest])) {
                    thread_id[omp_get_thread_num()] = j;
                    thread_min[omp_get_thread_num()] = distances[j];
                }
            }

            shortest = thread_id[std::min_element(thread_min.begin(), thread_min.end()) - thread_min.begin()];

            if (distances[shortest] == INF)
                break;
            visited[shortest] = true;

            graph_size = graph[shortest].size();
            if (chunkSize == -1) {
                chunkSize = graph_size / omp_get_max_threads();
            }

#pragma omp parallel for schedule(static, chunkSize) firstprivate(shortest, graph_size) private(dest, len, path_len, j) 
            for (j = 0; j < graph_size; ++j) {
                dest = graph[shortest][j].first;
                len = graph[shortest][j].second;
                path_len = distances[shortest] + len;
                if (path_len < distances[dest]) {
                    distances[dest] = path_len;
                    parents[dest] = shortest;
                }
            }
        }
        return distances;
	}

    vector<int> dijkstraDynamic(graph graph, long startNode, long chunkSize) {

        long j, path_len, len, dest, graph_size, shortest;
        long nodes = graph.size();

        vector<int> distances(nodes, INF), parents(nodes);
        vector<char> visited(nodes);
        vector<int> thread_min(omp_get_max_threads(), INF);
        vector<long> thread_id(omp_get_max_threads(), nodes + 1);
        distances[startNode] = 0;
        if (chunkSize == -1) {
            chunkSize = nodes / omp_get_max_threads();
        }

        for (int i = 0; i < nodes; ++i) {
            fill(thread_min.begin(), thread_min.end(), INF);
            fill(thread_id.begin(), thread_id.end(), nodes + 1);
#pragma omp parallel for schedule(dynamic, chunkSize) firstprivate(nodes) private(shortest, j)
            for (j = 0; j < nodes; ++j) {
                shortest = thread_id[omp_get_thread_num()];
                if (!visited[j] && (shortest == nodes + 1
                    || distances[j] < distances[shortest])) {
                    thread_id[omp_get_thread_num()] = j;
                    thread_min[omp_get_thread_num()] = distances[j];
                }
            }

            shortest = thread_id[min_element(thread_min.begin(), thread_min.end()) - thread_min.begin()];

            if (distances[shortest] == INF)
                break;
            visited[shortest] = true;

            graph_size = graph[shortest].size();
            if (chunkSize == -1) {
                chunkSize = graph_size / omp_get_max_threads();
            }

#pragma omp parallel for schedule(dynamic, chunkSize) firstprivate(shortest, graph_size) private(dest, len, path_len, j) 
            for (j = 0; j < graph_size; ++j) {
                dest = graph[shortest][j].first;
                len = graph[shortest][j].second;
                path_len = distances[shortest] + len;
                if (path_len < distances[dest]) {
                    distances[dest] = path_len;
                    parents[dest] = shortest;
                }
            }
        }
        return distances;
    }
}