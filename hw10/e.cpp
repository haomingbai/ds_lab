#include <iostream>
#include <vector>
#include <stack>
#include <utility>  // for std::pair
#include <algorithm> // for std::swap

// Lomuto 分区：返回枢轴最终索引
int partition_lomuto(std::vector<int>& arr, int low, int high) {
    int pivot = arr[high];
    int i = low - 1;  // i 指向小于等于 pivot 部分的末尾
    for (int j = low; j < high; ++j) {
        if (arr[j] <= pivot) {
            ++i;
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[high]);
    return i + 1;
}

// 非递归快速排序（升序），对 arr[0..n-1] 生效
void quicksort_nonrecursive(std::vector<int>& arr) {
    int n = static_cast<int>(arr.size());
    if (n < 2) return;

    // 栈里保存待排序区间 [low, high]
    std::stack<std::pair<int,int>> st;
    st.push({0, n - 1});

    while (!st.empty()) {
        auto [low, high] = st.top();
        st.pop();

        if (low < high) {
            int p = partition_lomuto(arr, low, high);
            // p 位置已经是正确的，左区间是 [low, p-1]，右区间是 [p+1, high]

            // 先后压栈顺序可以任意；一般推荐“先压较大区间，后压较小区间”，
            // 以减小栈深度。但这里简单地都压，只要检查区间长度 > 1 即可。
            if (p - 1 > low) {
                st.push({low, p - 1});
            }
            if (p + 1 < high) {
                st.push({p + 1, high});
            }
        }
    }
}

int main() {
    // 示例用法：
    std::vector<int> data = { 33, 10, 55, 71, 29, 3, 18, 95, 42 };

    std::cout << "排序前：";
    for (int x : data) {
        std::cout << x << " ";
    }
    std::cout << "\n";

    quicksort_nonrecursive(data);

    std::cout << "排序后：";
    for (int x : data) {
        std::cout << x << " ";
    }
    std::cout << "\n";

    return 0;
}
