#include <iostream>
#include <vector>
#include <stack>

using namespace std;

/**
 * @brief 非递归深度优先遍历（DFS）
 * @param G 邻接表存储的图
 * @param v 起始顶点编号
 */
void nonRecursiveDFS(const vector<vector<int>>& G, int v) {
    int n = G.size();
    if (v < 0 || v >= n) return;

    vector<bool> visited(n, false);
    stack<int> stk;
    stk.push(v);  // 初始顶点压栈

    while (!stk.empty()) {
        int u = stk.top();
        stk.pop();

        if (!visited[u]) {
            cout << u << " ";  // 访问顶点u
            visited[u] = true;

            // 逆序压入邻接点（保证递归顺序一致）
            for (auto it = G[u].rbegin(); it != G[u].rend(); ++it) {
                int w = *it;
                stk.push(w);
            }
        }
    }
}

int main() {
    // 示例邻接表
    vector<vector<int>> G = {
        {1, 2, 3},   // 顶点0的邻接点
        {0, 4},      // 顶点1的邻接点
        {0, 5},      // 顶点2的邻接点
        {0, 6},      // 顶点3的邻接点
        {1},         // 顶点4的邻接点
        {2},         // 顶点5的邻接点
        {3}          // 顶点6的邻接点
    };

    cout << "非递归DFS遍历结果（从顶点0开始）: ";
    nonRecursiveDFS(G, 0);  // 输出: 0 1 4 2 5 3 6
    return 0;
}
