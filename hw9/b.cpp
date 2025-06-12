#include <iostream>
#include <vector>
#include <cassert>

/// 二分查找：在已升序的整数数组 arr 中查找 key
/// 如果找到，就返回一个 vector，其中依次存放每轮比较时访问到的 arr[mid] 值
/// 如果没找到，则返回一个空的 vector。
std::vector<int> binary_search_with_path(const std::vector<int>& arr, int key) {
    std::vector<int> path; 
    int left  = 0;
    int right = static_cast<int>(arr.size()) - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        // 先把这轮访问到的元素值记录下来
        path.push_back(arr[mid]);

        if (arr[mid] == key) {
            // 找到了，直接返回 path
            return path;
        }
        else if (arr[mid] < key) {
            left = mid + 1;
        }
        else {
            right = mid - 1;
        }
    }

    // 如果跑到这里，说明没找到，返回空的 path
    return {};
}

int main() {
    // 示例有序表（升序）
    std::vector<int> A = { 2, 5, 8, 12, 16, 23, 38, 56, 72, 91 };
    // 要查找的关键字
    int k = 23;

    auto seq = binary_search_with_path(A, k);
    if (!seq.empty()) {
        std::cout << "成功找到 " << k << "，查找路径（依次访问的元素）为：";
        for (int v : seq) {
            std::cout << v << " ";
        }
        std::cout << "\n";
    }
    else {
        std::cout << "未能找到关键字 " << k << "。\n";
    }

    // 再做几个断言测试，确保行为符合预期
    {
        // 如果 k = 23，在上面这个数组中，索引为 5。mid 的计算依次为：
        //   left=0, right=9 -> mid=4 (访问 A[4]=16)
        //   16 < 23，所以 left=5, right=9 -> mid=7 (访问 A[7]=56)
        //   56 > 23，所以 left=5, right=6 -> mid=5 (访问 A[5]=23，命中)
        // 所以 path 应该是 [16, 56, 23]
        std::vector<int> expected = { 16, 56, 23 };
        assert(seq == expected);
    }

    {
        // 如果换一个存在但在最左边的，比如 k = 2：
        auto seq2 = binary_search_with_path(A, 2);
        // 访问顺序：mid=4(A[4]=16)、16>2 -> next mid=1(A[1]=5)、5>2 -> next mid=0(A[0]=2)
        std::vector<int> expected2 = { 16, 5, 2 };
        assert(seq2 == expected2);
    }

    {
        // 如果换一个存在但在最右边的，比如 k = 91：
        auto seq3 = binary_search_with_path(A, 91);
        // 访问顺序：mid=4(16)<91 -> mid=7(56)<91 -> mid=8(72)<91 -> mid=9(91)
        std::vector<int> expected3 = { 16, 56, 72, 91 };
        assert(seq3 == expected3);
    }

    {
        // 如果查找一个不存在的值，比如 k = 15，则返回空序列
        auto seq4 = binary_search_with_path(A, 15);
        assert(seq4.empty());
    }

    std::cout << "所有测试通过。\n";
    return 0;
}

