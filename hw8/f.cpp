#include <iostream>
#include <vector>
#include <queue>

using namespace std;

/**
 * @brief 找到距离顶点v的最短路径最远的顶点
 * @param G 邻接表存储的无向连通图
 * @param v 起始顶点
 * @return 最远顶点的编号
 */
int findFarthestVertex(const vector<vector<int>>& G, int v) {
    int n = G.size();
    if (n == 0 || v < 0 || v >= n) return -1; // 边界检查

    vector<int> dist(n, -1); // 记录顶点到v的最短距离
    queue<int> q;

    dist[v] = 0;   // 起始顶点距离为0
    q.push(v);

    // BFS遍历图
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (int w : G[u]) { // 遍历u的所有邻接点
            if (dist[w] == -1) { // 若w未被访问过
                dist[w] = dist[u] + 1;
                q.push(w);
            }
        }
    }

    // 找到最大距离对应的顶点
    int max_dist = 0;
    int farthest_vertex = v;
    for (int i = 0; i < n; ++i) {
        if (dist[i] > max_dist) {
            max_dist = dist[i];
            farthest_vertex = i;
        }
    }

    return farthest_vertex;
}

// 示例测试
int main() {
    // 示例1：链式结构 0-1-2-3
    vector<vector<int>> G1 = {
        {1},       // 顶点0的邻接点
        {0, 2},    // 顶点1的邻接点
        {1, 3},    // 顶点2的邻接点
        {2}        // 顶点3的邻接点
    };
    cout << "最远顶点（链式图）: " << findFarthestVertex(G1, 0) << endl; // 输出3

    // 示例2：星型结构，中心为0
    vector<vector<int>> G2 = {
        {1, 2, 3}, // 顶点0的邻接点
        {0},        // 顶点1的邻接点
        {0},        // 顶点2的邻接点
        {0}         // 顶点3的邻接点
    };
    cout << "最远顶点（星型图）: " << findFarthestVertex(G2, 0) << endl; // 输出3（具体取决于邻接表顺序）

    return 0;
}
