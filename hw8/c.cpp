#include <vector>
#include <algorithm>

/**
 * @brief 删除顶点i到j的边（邻接表存储）
 * @param G 邻接表，G[i]存储顶点i的所有出边邻接点
 * @param i 边的起点
 * @param j 边的终点
 * @return 是否成功删除（若边存在则返回true）
 */
bool removeEdge(std::vector<std::vector<int>>& G, int i, int j) {
    // 检查顶点i和j的合法性
    if (i < 0 || i >= G.size() || j < 0 || j >= G.size()) {
        return false;
    }

    // 使用STL算法删除G[i]中所有等于j的元素（若需仅删除第一个，改用find+erase）
    auto& neighbors = G[i];
    auto it = std::remove(neighbors.begin(), neighbors.end(), j);
    bool removed = (it != neighbors.end());
    neighbors.erase(it, neighbors.end());

    return removed;
}

int main() {
    // 示例邻接表
    std::vector<std::vector<int>> G = {
        {1, 2, 3},   // 顶点0的出边
        {2},         // 顶点1的出边
        {0},         // 顶点2的出边
        {0, 2}       // 顶点3的出边
    };

    // 删除顶点0到2的边
    removeEdge(G, 0, 2);

    // 验证结果：G[0]应变为[1, 3]
    return 0;
}
