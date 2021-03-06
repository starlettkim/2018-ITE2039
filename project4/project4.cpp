#include <fstream>
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <iostream>
#include <cstring>

using namespace std;

using ull   = unsigned long long;
using vtx_t = int;
using edg_t = unsigned int;
using dis_t = unsigned int;
using p_dv  = pair< dis_t, vtx_t >;

const vtx_t MAX_VTX_NUM   = 1e7;
const dis_t INF           = 2e9 + 1;


vector< p_dv > g_graph[MAX_VTX_NUM];

vtx_t g_num_vertices;
edg_t g_num_edges;


inline void run_dijkstra(dis_t * ret, vtx_t st, const bool * is_target, dis_t * distance) {
    priority_queue< p_dv, vector< p_dv >, greater< p_dv > > pq;

    // Initialize
    pq.push({ 0, st });

    // Main loop
    while (!pq.empty()) {
        // Pop a vertex in pq, then its distance is fixed.
        dis_t now_dis = pq.top().first;
        vtx_t now_vtx = pq.top().second;
        pq.pop();

        // If we already have better result, no more work needed.
        if (*ret <= now_dis) {
            return;
        }
        // Found target with better result.
        else if (is_target[now_vtx] && now_vtx != st) {
            *ret = now_dis;
            return;
        }

        // Check if this vertex is already visited.
        if (distance[now_vtx] < now_dis) {
            continue;
        }
        // Proceed.
        for (auto i : g_graph[now_vtx]) {
            dis_t nxt_edg = i.first;
            vtx_t nxt_vtx = i.second;
            #define IS_INF(VTX) ((VTX) != st && !distance[(VTX)])
            if (distance[nxt_vtx] > now_dis + nxt_edg || IS_INF(nxt_vtx)) {
                distance[nxt_vtx] = now_dis + nxt_edg;
                pq.push({ distance[nxt_vtx], nxt_vtx });
            }
            #undef IS_INF
        }
    }
}

void find_shortest_distance(dis_t * ret, vector< vtx_t > target_vertices) {
    bool *  is_target = new bool[g_num_vertices]();
    dis_t * distance  = new dis_t[g_num_vertices];
    for (vtx_t v : target_vertices) {
        is_target[v] = true;
    }
    *ret = INF;
    for (vtx_t v : target_vertices) {
        memset(distance, 0, sizeof(dis_t) * g_num_vertices);
        run_dijkstra(ret, v, is_target, distance);
    }
    delete[] distance;
    delete[] is_target;
}

int main(int argc, char * argv[]) {
    // Open input file.
    ifstream ifs(( argc == 1 ? "input.txt" : argv[1] ), ifstream::in);
    ofstream ofs("output.txt", ofstream::out);

    // Construct an adjacent list.
    ifs >> g_num_vertices >> g_num_edges;
    for (int i = 0; i < g_num_edges; i++) {
        vtx_t v1, v2;
        dis_t d;
        ifs >> v1 >> v2 >> d;
        g_graph[v1].push_back({ d, v2 });
        g_graph[v2].push_back({ d, v1 });
    }
    
    // Parallel query execution
    thread * threads;
    dis_t  * result;
    int      P;      // Number of problems <= 100
    int      N;      // Number of houses <= 64

    ifs >> P;
    threads = new thread[100];
    result  = new dis_t[100];
    for (int i = 0; i < P; i++) {
        ifs >> N;
        vector< vtx_t > target_vertices(N);
        for (int j = 0; j < N; j++) {
            ifs >> target_vertices[j];
        }
        threads[i] = thread(find_shortest_distance, &result[i], target_vertices);
    }
    
    // Collect results.
    for (int i = 0; i < P; i++) {
        threads[i].join();
    }
    for (int i = 0; i < P; i++) {
        ofs << result[i] << '\n';
    }

    delete[] threads;
    delete[] result;

    return 0;
}