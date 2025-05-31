#include <iostream>
#include <vector>

/**
 * @brief 获取顶点i的所有入边邻接点（即指向顶点i的顶点）
 * @param g 邻接矩阵，g[u][v]表示u到v是否有边
 * @param i 目标顶点编号
 * @return vector<int> 包含所有入边邻接点的列表
 */
std::vector<int> findInNeighbors(const std::vector<std::vector<int>>& g, int i) {
    std::vector<int> in_neighbors;
    // 遍历所有顶点u（即所有行）
    for (int u = 0; u < g.size(); ++u) {
        // 检查邻接矩阵中第u行、第i列是否为1
        if (g[u][i] == 1) {
            in_neighbors.push_back(u);
        }
    }
    return in_neighbors;
}

int main() {
    // 示例邻接矩阵（4个顶点）
    std::vector<std::vector<int>> g = {
        {0, 1, 1, 0},  // 顶点0的出边
        {0, 0, 1, 0},  // 顶点1的出边
        {1, 0, 0, 0},  // 顶点2的出边
        {1, 0, 1, 0}   // 顶点3的出边
    };

    // 测试顶点i=2的入边邻接点
    int target = 2;
    std::vector<int> result = findInNeighbors(g, target);

    // 输出结果
    std::cout << "顶点" << target << "的入边邻接点: ";
    for (int u : result) {
        std::cout << u << " ";
    }
    std::cout << std::endl;  // 输出: 0 1 3

    return 0;
}
