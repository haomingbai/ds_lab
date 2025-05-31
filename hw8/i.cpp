#include <bits/stdc++.h>
using namespace std;

// 全局变量或传引用
vector<vector<int>> G;      // 邻接表表示的无向图
vector<bool> visited;       // 访问标记
vector<bool> inStack;       // 递归栈标记
vector<int> path;           // 当前路径
int found = 0;              // 标志是否已找到环

// 输出并退出
void outputCycle(int w) {
    vector<int> cycle;
    // 从 path 中找到第一个等于 w 的位置
    auto it = find(path.begin(), path.end(), w);
    for (; it != path.end(); ++it) {
        cycle.push_back(*it);
    }
    cycle.push_back(w); // 回到起点
    // 打印
    cout << "Found cycle:";
    for (int x : cycle) {
        cout << " " << x;
    }
    cout << "\n";
    found = 1;
}

// 从 u 开始 DFS
void dfs(int u) {
    if (found) return;            // 已经找到就不再搜索
    visited[u] = true;
    inStack[u] = true;
    path.push_back(u);

    for (int w : G[u]) {
        if (found) break;
        if (!visited[w]) {
            dfs(w);
        } else if (inStack[w]) {
            // 遇到回边，输出一次经过 w 的环
            outputCycle(w);
            break;
        }
    }

    // 回溯
    inStack[u] = false;
    path.pop_back();
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m, v;
    // n: 顶点数 (0..n-1)， m: 边数， v: 指定顶点
    cin >> n >> m >> v;
    G.assign(n, {});
    visited.assign(n, false);
    inStack.assign(n, false);

    for (int i = 0; i < m; i++) {
        int a, b;
        cin >> a >> b;
        // 假设顶点编号是 0..n-1
        G[a].push_back(b);
        G[b].push_back(a);
    }

    dfs(v);
    if (!found) {
        cout << "No simple cycle passing through vertex " << v << "\n";
    }
    return 0;
}

