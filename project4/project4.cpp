#include <fstream>
#include <thread>
#include <cassert>
#include <vector>

using ull   = unsigned long long;
using vtx_t = int;
using edg_t = int;
using dis_t = int;

const vtx_t MAX_VTX_NUM   = 1e7;
const int   MAX_QUERY_NUM = 100;

vector< pair< vtx_t, dis_t > > g_graph[MAX_VTX_NUM];
dis_t g_result[MAX_QUERY_NUM];

using namespace std;

dis_t run_dijkstra(vtx_t st) {

}

void find_shortest_distance(vector< vtx_t > target_vertices) {
    dis_t min_distance;

}

int main(int argc, char * argv[]) {
    // Open input file.
    ifstream ifs(( argc == 1 ? "input.txt" : argv[1] ), ifstream::in);
    ofstream ofs("output.txt", ofstream::out);
    assert(ifs.good() && ofs.good());

    // Construct an adjacent list.
    vtx_t num_vertices;
    edg_t num_edges;
    ifs >> num_vertices >> num_edges;

    while (num_edges--) {
        vtx_t v1, v2;
        dis_t d;
        ifs >> v1 >> v2 >> d;
        g_graph[v1].push_back({ v2, d });
        g_graph[v2].push_back({ v1, d });
    }

    // Parallel query execution
    vector< thread > threads;
    int P;      // Number of problems <= 100
    ifs >> P;
    while (P--) {
        int N;      // Number of houses <= 64
        ifs >> N;
        vector< vtx_t > target_vertices;
        target_vertices.resize(N);
        for (int i = 0; i < N; i++) {
            ifs >> target_vertices[i];
        }
        threads.push_back(thread(find_shortest_distance, target_vertices));
    }

    // Collect results.
    for (auto & thr : threads) {
        thr.join();
    }

    return 0;
}