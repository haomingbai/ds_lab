#include <bits/stdc++.h>
using namespace std;

// 全局变量
int n, m, v;                                  // 顶点数、边数、指定顶点
vector<vector<int>> G;                        // 邻接表
vector<bool> visited;                         // 访问标记
vector<int> path_;                            // 当前回溯路径（不含终点 v）
set<vector<int>> seen_cycles;                 // 存放已输出的规范化回路

// 将 cycle 规范化（左旋到最小元素做首，取正反中字典序小者）
vector<int> normalize_cycle(const vector<int>& cycle) {
    int L = cycle.size();
    // 找出最小顶点下标
    int minv = *min_element(cycle.begin(), cycle.end());
    vector<int> best;
    // 尝试所有旋转
    for (int shift = 0; shift < L; ++shift) {
        if (cycle[shift] != minv) continue;
        // 正向旋转
        vector<int> tmp(L);
        for (int i = 0; i < L; ++i)
            tmp[i] = cycle[(shift + i) % L];
        if (best.empty() || tmp < best) best = tmp;
        // 反向旋转
        vector<int> tmp2(L);
        for (int i = 0; i < L; ++i)
            tmp2[i] = cycle[(shift - i + L) % L];
        if (tmp2 < best) best = tmp2;
    }
    return best;
}

// 在 path_ 上继续回溯，枚举所有从当前 u 回到 v 的简单回路
void dfs(int u) {
    visited[u] = true;
    path_.push_back(u);

    for (int w : G[u]) {
        if (w == v) {
            // 找到一条回路：v -> path_ -> v
            vector<int> cycle;
            cycle.push_back(v);
            cycle.insert(cycle.end(), path_.begin(), path_.end());
            cycle.push_back(v);
            auto norm = normalize_cycle(cycle);
            if (seen_cycles.insert(norm).second) {
                // 新回路，输出
                for (int x : norm)
                    cout << x << ' ';
                cout << '\n';
            }
        }
        else if (!visited[w] && w > v) {
            // 仅当 w > v 时继续（可进一步减少对称重复，选用或不选用均可）
            dfs(w);
        }
    }

    visited[u] = false;
    path_.pop_back();
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> n >> m >> v;
    G.assign(n, {});
    for (int i = 0; i < m; i++) {
        int a, b;
        cin >> a >> b;
        G[a].push_back(b);
        G[b].push_back(a);
    }

    // 初始化
    visited.assign(n, false);
    visited[v] = true;   // 起点 v 标记为已访问，防止中途重入
    // 对 v 的每个邻居启动一次回溯
    for (int u : G[v]) {
        if (!visited[u]) {
            dfs(u);
        }
    }

    // 若没有任何回路输出，可提示
    if (seen_cycles.empty()) {
        cout << "No simple cycle passing through vertex " << v << "\n";
    }

    return 0;
}

