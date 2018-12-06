#include <fstream>
#include <thread>
#include <cassert>
#include <vector>

typedef unsigned long long ull;

using namespace std;

uint64_t g_V;    // number of vertices
uint64_t g_E;    // number of edges

vector< vector< pair< uint64_t, uint64_t > > > g_graph;
// g_graph[edge_no][i] = {connected_edge_no, distance}

void find_shortest_distance(vector< uint64_t > & target_vertices) {
    
}

int main(int argc, char * argv[]) {
    ifstream ifs(( argc == 1 ? "input.txt" : argv[1] ), ifstream::in);
    ofstream ofs("output.txt", ofstream::out);
    assert(ifs.good() && ofs.good());

    ifs >> g_V >> g_E;
    g_graph.resize(g_V);

    for (int i = 0; i < g_E; i++) {
        uint64_t v1, v2, d;
        ifs >> v1 >> v2 >> d;
        g_graph[v1 - 1].push_back({ v2 - 1, d });
        g_graph[v2 - 1].push_back({ v1 - 1, d });
    }

    vector< thread > threads;
    uint64_t P;

    ifs >> P;
    while (P--) {
        uint64_t N;
        vector< uint64_t > target_vertices;
        ifs >> N;
        target_vertices.resize(N);
        for (int i = 0; i < N; i++) {
            ifs >> target_vertices[i];
        }
        threads.push_back(thread(find_shortest_distance, target_vertices));
    }
    // 1. Dijkstra
    // 2. Floyd-Warshall

    return 0;
}