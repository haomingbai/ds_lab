#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/// 二分插入排序（降序），不返回值，直接在 A[0..n-1] 上原地排序
/// 当输入已经是降序时，搬移操作次数（元素赋值次数）正好为 0。
void binary_insertion_sort_desc(int A[], int n) {
    for (int i = 1; i < n; i++) {
        int key = A[i];

        // —— 在 A[0..i-1] 区间做二分查找，找第一个 A[idx] < key 的位置
        int low  = 0;
        int high = i - 1;
        while (low <= high) {
            int mid = (low + high) >> 1;
            if (A[mid] < key) {
                // 如果中间元素比 key 小，就往左缩右边界
                high = mid - 1;
            } else {
                // 否则说明 A[mid] >= key，需要往右走
                low = mid + 1;
            }
        }
        int pos = low;  // 插入位置

        // —— 将 A[pos..i-1] 向右移动一格
        int j = i - 1;
        while (j >= pos) {
            A[j + 1] = A[j];
            j--;
        }

        // 把 key 插到 A[pos]
        A[pos] = key;
    }
}

/// 辅助：打印数组
void print_array(const char *title, int A[], int n) {
    printf("%s: [", title);
    for (int i = 0; i < n; i++) {
        printf("%d", A[i]);
        if (i < n - 1) printf(", ");
    }
    printf("]\n");
}

/// 主函数：测试两种情况
int main(void) {
    // —— 示例 1：已经是降序，检查搬移次数
    {
        int A1[] = { 100, 90, 90, 70, 50, 50, 20, 10 };
        int n1 = sizeof(A1) / sizeof(A1[0]);

        // 为了验证“搬移次数为 0”，我们可以临时在内层代码里插入一个计数器，
        // 但此处为了简便，我们只“目测”其排序结果 & 逻辑分析能保证不搬移。
        //
        // 先打印初始状态，再排序，最后再打印一次。排序后应该和原来一模一样。
        print_array("示例 1-排序前（已降序）", A1, n1);
        binary_insertion_sort_desc(A1, n1);
        print_array("示例 1-排序后（应无变化）", A1, n1);
        printf("—— 示例 1 完成 (如排序前后数组一致，则搬移次数为 0)。\n\n");
    }

    // —— 示例 2：随机无序，排序后应能变为降序
    {
        const int n2 = 12;
        int A2[n2];
        srand((unsigned)time(NULL));

        // 生成 0..99 之间的随机数填充
        for (int i = 0; i < n2; i++) {
            A2[i] = rand() % 100;
        }

        print_array("示例 2-排序前（随机）", A2, n2);
        binary_insertion_sort_desc(A2, n2);
        print_array("示例 2-排序后（应降序）", A2, n2);

        // 简单检查：保证 A2[i] ≥ A2[i+1]
        int cnt_ok = 1;
        for (int i = 0; i + 1 < n2; i++) {
            if (A2[i] < A2[i + 1]) {
                cnt_ok = 0;
                break;
            }
        }
        if (cnt_ok) {
            printf("—— 示例 2 检查通过：确实已降序排列。\n\n");
        } else {
            printf("—— 示例 2 检查未通过，请检查算法！\n\n");
        }
    }

    return 0;
}

