#include <iostream>
#include <vector>
#include <queue>

using namespace std;

/**
 * @brief 使用深度优先搜索（DFS）遍历连通分量
 * @param G 邻接表
 * @param u 当前顶点
 * @param visited 访问标记数组
 */
void dfs(const vector<vector<int>>& G, int u, vector<bool>& visited) {
    visited[u] = true;
    for (int v : G[u]) {
        if (!visited[v]) {
            dfs(G, v, visited);
        }
    }
}

/**
 * @brief 计算无向图的连通分量个数
 * @param G 邻接表
 * @return 连通分量的数量
 */
int countConnectedComponents(const vector<vector<int>>& G) {
    int n = G.size();
    vector<bool> visited(n, false);
    int count = 0;
    
    for (int u = 0; u < n; ++u) {
        if (!visited[u]) {
            dfs(G, u, visited);  // 也可替换为BFS
            count++;
        }
    }
    return count;
}

// 可选：BFS实现
void bfs(const vector<vector<int>>& G, int u, vector<bool>& visited) {
    queue<int> q;
    q.push(u);
    visited[u] = true;
    while (!q.empty()) {
        int curr = q.front();
        q.pop();
        for (int v : G[curr]) {
            if (!visited[v]) {
                visited[v] = true;
                q.push(v);
            }
        }
    }
}

int main() {
    // 示例邻接表
    vector<vector<int>> G = {
        {1, 2},    // 顶点0的邻居
        {0, 2},    // 顶点1的邻居
        {0, 1},    // 顶点2的邻居
        {}          // 顶点3的邻居（孤立顶点）
    };

    int components = countConnectedComponents(G);
    cout << "连通分量个数: " << components << endl;  // 输出: 2
    return 0;
}
