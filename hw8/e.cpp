#include <iostream>
#include <vector>
#include <queue>

using namespace std;

/**
 * @brief 使用BFS判断顶点i到j是否存在路径（邻接矩阵存储）
 * @param g 邻接矩阵，g[u][v]=1表示存在边u→v
 * @param i 起点顶点编号
 * @param j 终点顶点编号
 * @return true表示存在路径，false表示无路径
 */
bool hasPathBFS(const vector<vector<int>>& g, int i, int j) {
    int n = g.size();
    // 校验顶点合法性
    if (i < 0 || i >= n || j < 0 || j >= n) {
        return false;
    }
    // 顶点i和j为同一顶点
    if (i == j) {
        return true;
    }

    vector<bool> visited(n, false); // 访问标记数组
    queue<int> q;                    // BFS队列

    // 初始化：起点入队并标记
    q.push(i);
    visited[i] = true;

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        // 遍历u的所有邻接点v
        for (int v = 0; v < n; ++v) {
            if (g[u][v] == 1) {      // 存在边u→v
                if (v == j) {
                    return true;     // 找到目标顶点j
                }
                if (!visited[v]) {   // v未被访问过
                    visited[v] = true;
                    q.push(v);       // v入队
                }
            }
        }
    }

    return false; // 遍历完未找到路径
}

int main() {
    // 示例邻接矩阵（4个顶点）
    vector<vector<int>> g = {
        {0, 1, 1, 0},  // 顶点0的出边
        {0, 0, 1, 0},  // 顶点1的出边
        {0, 0, 0, 1},  // 顶点2的出边
        {0, 0, 0, 0}   // 顶点3的出边
    };

    int i = 0, j = 3;
    if (hasPathBFS(g, i, j)) {
        cout << "存在路径 " << i << " → " << j << endl; // 输出：存在路径 0 → 3
    } else {
        cout << "无路径" << endl;
    }

    return 0;
}
