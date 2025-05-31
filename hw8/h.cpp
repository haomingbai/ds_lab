#include <iostream>
#include <vector>
using namespace std;

/**
 * @brief DFS辅助函数，检测是否存在包含顶点target的环
 * @param G 邻接表
 * @param u 当前顶点
 * @param visited 访问标记数组
 * @param parent 父节点数组
 * @param target 目标顶点v
 * @return true表示存在包含v的环，false表示无环
 */
bool dfs(const vector<vector<int>>& G, int u, vector<bool>& visited, vector<int>& parent, int target) {
    visited[u] = true;
    for (int w : G[u]) {
        if (!visited[w]) {
            parent[w] = u;  // 记录父节点
            if (dfs(G, w, visited, parent, target)) {
                return true;
            }
        } else if (parent[u] != w) {  // 发现回边（w不是父节点）
            // 回溯父节点链，检查环中是否包含target
            int current = u;
            while (current != w && current != -1) {
                if (current == target) {
                    return true;  // 环包含target
                }
                current = parent[current];
            }
            // 检查终点w是否是target
            if (w == target) return true;
        }
    }
    return false;
}

/**
 * @brief 判断无向图中是否存在经过顶点v的简单回路
 * @param G 邻接表
 * @param v 目标顶点
 * @return true表示存在包含v的环，false表示无环
 */
bool hasCycleThroughV(const vector<vector<int>>& G, int v) {
    int n = G.size();
    if (v < 0 || v >= n) return false;

    vector<bool> visited(n, false);
    vector<int> parent(n, -1);  // 父节点初始化为-1
    visited[v] = true;          // 标记起点为已访问

    // 遍历顶点v的所有邻接点
    for (int w : G[v]) {
        if (!visited[w]) {
            parent[w] = v;
            if (dfs(G, w, visited, parent, v)) {
                return true;
            }
        } else {
            // 处理v的邻接点w已被访问的情况（可能形成环v-w-v）
            if (w != parent[v]) {  // 避免将无向边v-w误判为环
                // 检查环是否包含v（直接判断w是否等于v）
                if (w == v) return true;  // 自环（需根据题目定义决定是否允许）
                // 回溯w的父节点链，检查是否包含v
                int current = w;
                while (parent[current] != -1) {
                    if (parent[current] == v) return true;
                    current = parent[current];
                }
            }
        }
    }
    return false;
}

// 示例测试
int main() {
    // 示例1：包含顶点0的环
    vector<vector<int>> G1 = {
        {1, 2},    // 顶点0的邻接点
        {0, 2},    // 顶点1的邻接点
        {0, 1, 3}, // 顶点2的邻接点
        {2}        // 顶点3的邻接点
    };
    cout << "示例1是否存在包含顶点0的环: " << hasCycleThroughV(G1, 0) << endl; // 输出1（true）

    // 示例2：无环
    vector<vector<int>> G2 = {
        {1},       // 顶点0的邻接点
        {0, 2},    // 顶点1的邻接点
        {1, 3},    // 顶点2的邻接点
        {2}        // 顶点3的邻接点
    };
    cout << "示例2是否存在包含顶点0的环: " << hasCycleThroughV(G2, 0) << endl; // 输出0（false）

    return 0;
}
