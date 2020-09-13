#include <vector>
#include <omp.h>
#include <algorithm>
#include <climits>

#define INF INT32_MAX

using namespace std;

namespace dijkstra {
    using graph = vector<vector<pair<int, int>>>;

    vector<int> dijkstraStatic(graph graph, long startNode, long chunkSize) {

        long j, path_len, len, dest, graph_size, shortest;
        long nodes = graph.size();

        vector<int> distances(nodes, INF);
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
                }
            }
        }
        return distances;
    }

    vector<int> dijkstraDynamic(graph graph, long startNode, long chunkSize) {

        long j, path_len, len, dest, graph_size, shortest;
        long nodes = graph.size();

        vector<int> distances(nodes, INF);
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
                }
            }
        }
        return distances;
    }

    struct node {
        int distance;
        int vertex;
    };

    vector<int> dijkstraReduction(graph graph, int startNode, int nodes) {

        int j, path_len, len;
        int dest, graph_size;
        vector<int> distances(nodes, INF);
        vector<int> visited(nodes, 0);
        distances[startNode] = 0;
        
#pragma omp declare reduction(min : node: \
	omp_out.distance = omp_in.distance > omp_out.distance ? omp_out.distance:omp_in.distance,\
	omp_out.vertex   = omp_in.distance > omp_out.distance ? omp_out.vertex:omp_in.vertex) \
	initializer(omp_priv={INF,0})

        for (int i = 0; i < nodes; i++) {

            node shortest = { INF, startNode };
#pragma omp parallel for firstprivate(nodes) private(j) reduction(min : shortest)
            for (j = 0; j < nodes; ++j) {
                if (!visited[j] && (shortest.vertex == nodes + 1
                    || distances[j] < distances[shortest.vertex])) {
                    shortest.vertex = j;
                    shortest.distance = distances[j];
                }
            }

            if (distances[shortest.vertex] == INF)
                break;
            visited[shortest.vertex] = true;

            graph_size = graph[shortest.vertex].size();

#pragma omp parallel for firstprivate(shortest, graph_size) private(dest, len, path_len, j) 
            for (j = 0; j < graph_size; ++j) {
                dest = graph[shortest.vertex][j].first;
                len = graph[shortest.vertex][j].second;
                path_len = distances[shortest.vertex] + len;
                if (path_len < distances[dest]) {
                    distances[dest] = path_len;
                }
            }
        }
        return distances;
    }
}