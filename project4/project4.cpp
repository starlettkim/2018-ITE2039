#include <fstream>
#include <thread>
#include <cassert>
#include <vector>
#include <queue>
#include <functional>
#include <iostream>

using namespace std;

using ull   = unsigned long long;
using vtx_t = int;
using edg_t = unsigned int;
using dis_t = unsigned int;

const vtx_t MAX_VTX_NUM   = 1e7;
const dis_t INF           = 2e9 + 1;


vector< pair< vtx_t, dis_t > > g_graph[MAX_VTX_NUM];

vtx_t g_num_vertices;
edg_t g_num_edges;


void run_dijkstra(dis_t * ret, vtx_t st, vector< bool > & chk) {
    vector< dis_t > distance(g_num_vertices, INF);
    using p_dv = pair< dis_t, vtx_t >;
    priority_queue< p_dv, vector< p_dv >, greater< p_dv > > pq;

    // Initialize
    pq.push({ 0, st });
    distance[0] = 0;

    // Main loop
    while (!pq.empty()) {
        // Pop a vertex in pq, then its distance is fixed.
        dis_t now_dis = pq.top().first;
        vtx_t now_vtx = pq.top().second;
        pq.pop();
        // Found another target vertex. Update the result.
        if (now_vtx != st && chk[now_vtx]) {
            *ret = now_dis;
            return;
        // If we already have better result, no more work needed.
        } else if (*ret <= now_dis) {
            return;
        }

        // Proceed.
        for (auto i : g_graph[now_vtx]) {
            vtx_t nxt_vtx = i.first;
            dis_t nxt_edg = i.second;
            if (distance[nxt_vtx] > now_dis + nxt_edg) {
                distance[nxt_vtx] = now_dis + nxt_edg;
                pq.push({ distance[nxt_vtx], nxt_vtx });
            }
        }
    }
}

void find_shortest_distance(dis_t * ret, vector< vtx_t > target_vertices) {
    vector< bool > chk;     // Check whether it is a target vertex(house).
    chk.resize(g_num_vertices);
    for (vtx_t v : target_vertices) {
        chk[v] = true;
    }
    for (vtx_t v : target_vertices) {
        run_dijkstra(ret, v, chk);
    }
}

int main(int argc, char * argv[]) {
    // Open input file.
    ifstream ifs(( argc == 1 ? "small.dms" : argv[1] ), ifstream::in);
    ofstream ofs("output.txt", ofstream::out);
    assert(ifs.good() && ofs.good());

    // Construct an adjacent list.
    ifs >> g_num_vertices >> g_num_edges;
    for (int i = 0; i < g_num_edges; i++) {
        vtx_t v1, v2;
        dis_t d;
        ifs >> v1 >> v2 >> d;
        g_graph[v1].push_back({ v2, d });
        g_graph[v2].push_back({ v1, d });
    }
    
    // Parallel query execution
    vector< thread > threads;
    int P;      // Number of problems <= MAX_QUERY_NUM
    int N;      // Number of houses <= 64
    ifs >> P;
    vector< dis_t > result(P, INF);
    for (int i = 0; i < P; i++) {
        ifs >> N;
        vector< vtx_t > target_vertices(N);
        for (int j = 0; j < N; j++) {
            ifs >> target_vertices[j];
        }
        threads.push_back(thread(find_shortest_distance, &result[i], target_vertices));
    }
    
    // Collect results.
    for (auto & thr : threads) {
        thr.join();
    }
    for (int i = 0; i < P; i++) {
        ofs << result[i] << '\n';
    }

    return 0;
}