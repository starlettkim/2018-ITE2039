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
using p_dv  = pair< dis_t, vtx_t >;

const vtx_t MAX_VTX_NUM   = 1e7;
const dis_t INF           = 2e9 + 1;


vector< p_dv > g_graph[MAX_VTX_NUM];

vtx_t g_num_vertices;
edg_t g_num_edges;


void run_dijkstra(dis_t * ret, vtx_t st, bool * is_target) {
    vector< dis_t > distance(g_num_vertices, INF);
    priority_queue< p_dv, vector< p_dv >, greater< p_dv > > pq;

    // Initialize
    pq.push({ 0, st });
    distance[st] = 0;

    // Main loop
    while (!pq.empty()) {
        // Pop a vertex in pq, then its distance is fixed.
        dis_t now_dis = pq.top().first;
        vtx_t now_vtx = pq.top().second;
        pq.pop();

        // Already visited this vertex.
        // if (chk[now_vtx] == 2) {
        //     continue;
        // }
        // Found another target vertex. Update the result and return.
        if (now_vtx != st && is_target[now_vtx]) {
            *ret = now_dis;
            return;
        // If we already have better result, no more work needed.
        } else if (*ret <= now_dis) {
            return;
        }

        // Proceed.
        // chk[now_vtx] = 2;
        for (auto i : g_graph[now_vtx]) {
            dis_t nxt_edg = i.first;
            vtx_t nxt_vtx = i.second;
            if (distance[nxt_vtx] > now_dis + nxt_edg) {
                distance[nxt_vtx] = now_dis + nxt_edg;
                pq.push({ distance[nxt_vtx], nxt_vtx });
            }
        }
    }
}

void find_shortest_distance(dis_t * ret, vector< vtx_t > target_vertices) {
    bool * is_target = new bool[g_num_vertices];
    for (vtx_t v : target_vertices) {
        target_vertices[v] = true;
    }
    for (vtx_t v : target_vertices) {
        run_dijkstra(ret, v, is_target);
    }
}

int main(int argc, char * argv[]) {
    // Open input file.
    ifstream ifs(( argc == 1 ? "input.txt" : argv[1] ), ifstream::in);
    ofstream ofs("output.txt", ofstream::out);
    assert(ifs.good() && ofs.good());

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
    threads = new thread[P];
    result  = new dis_t[P];
    fill(result, result + P, INF);
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

    return 0;
}