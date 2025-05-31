#include <bits/stdc++.h>
using namespace std;

// 全局变量
int n, m;                       // 顶点数、边数
int u, v, k;                    // 起点u，终点v，禁入点k
vector<vector<int>> G;          // 邻接表
vector<bool> visited;           // 访问标记
vector<int> path;               // 当前路径
vector<vector<int>> allPaths;   // 存放所有符合要求的路径

// 回溯搜索
void dfs(int x) {
    if (x == k) return;         // 不允许经过 k
    visited[x] = true;
    path.push_back(x);

    if (x == v) {
        allPaths.push_back(path);   // 找到一条从 u 到 v 的合法路径
    } else {
        for (int y : G[x]) {
            if (!visited[y]) {
                dfs(y);
            }
        }
    }

    // 回溯
    path.pop_back();
    visited[x] = false;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // 读入：n m u v k
    cin >> n >> m >> u >> v >> k;
    G.assign(n+1, {});
    visited.assign(n+1, false);

    // 读边（假定顶点编号为 1..n）
    for (int i = 0; i < m; i++) {
        int a, b;
        cin >> a >> b;
        G[a].push_back(b);
        G[b].push_back(a);
    }

    // 从 u 开始回溯
    dfs(u);

    // 输出结果
    if (allPaths.empty()) {
        cout << "No simple path from " << u << " to " << v
             << " without passing through " << k << "\n";
    } else {
        for (auto &p : allPaths) {
            for (int x : p) {
                cout << x << ' ';
            }
            cout << '\n';
        }
    }
    return 0;
}

