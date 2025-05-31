#include <bits/stdc++.h>
using namespace std;

const int INF = 0x3f3f3f3f;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, s, t;
    // 输入顶点数 n，源点 s，目标点 t
    cin >> n >> s >> t;

    // 邻接矩阵 W
    vector<vector<int>> W(n, vector<int>(n, INF));
    for (int i = 0; i < n; i++) {
        W[i][i] = 0;
    }

    int m;
    // 输入边数 m，接着 m 行 (u, v, w)
    cin >> m;
    for (int i = 0; i < m; i++) {
        int u, v, w;
        cin >> u >> v >> w;
        // 若为无向图，则两边都赋值；若有向图则只赋 W[u][v]
        W[u][v] = min(W[u][v], w);
        W[v][u] = min(W[v][u], w);
    }

    vector<int> dist(n, INF);
    vector<bool> vis(n, false);

    // 初始化
    for (int i = 0; i < n; i++) {
        dist[i] = W[s][i];
    }
    dist[s] = 0;
    vis[s] = true;

    // 主循环：寻找并“固定”剩余顶点
    for (int k = 1; k < n; k++) {
        int u = -1, minDist = INF;
        // 选取未访问且 dist 最小的顶点 u
        for (int i = 0; i < n; i++) {
            if (!vis[i] && dist[i] < minDist) {
                minDist = dist[i];
                u = i;
            }
        }
        if (u == -1) break;  // 剩余不可达

        vis[u] = true;
        // 松弛
        for (int vtx = 0; vtx < n; vtx++) {
            if (!vis[vtx] && W[u][vtx] < INF) {
                dist[vtx] = min(dist[vtx], dist[u] + W[u][vtx]);
            }
        }
    }

    // 输出结果
    if (dist[t] == INF) {
        cout << "No path from " << s << " to " << t << "\n";
    } else {
        cout << "Shortest distance from " << s << " to " << t
             << " is " << dist[t] << "\n";
    }

    return 0;
}

