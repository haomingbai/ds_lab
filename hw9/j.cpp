#include <iostream>
#include <vector>
#include <list>
#include <iomanip> // 用于设置输出格式

class HashTable {
public:
    HashTable(int size) 
        : TABLE_SIZE(size), table(size) {}

    // 哈希函数：除留余数法
    int hashFunc(int key) const {
        return key % TABLE_SIZE;
    }

    // 插入元素 k（如果已存在则不重复插入）
    void insert(int key) {
        int idx = hashFunc(key);
        for (int x : table[idx]) {
            if (x == key) return;
        }
        table[idx].push_back(key);
        ++numElements;
    }

    // 计算“成功检索平均检索长度”（ASL1）
    // ASL1 = (1 / n) * Σ_{i=0..TABLE_SIZE-1} [ L_i * (L_i + 1) / 2 ]
    // 其中 L_i 为桶 i 中的链长，n 为元素总数
    double ASL1() const {
        if (numElements == 0) return 0.0; // 空表则返回 0
        long long sumPositions = 0;
        // 对每个桶，链长为 L，若想检索成功，所有 L 个元素各自比较次数总和 = 1 + 2 + ... + L = L*(L+1)/2
        for (int i = 0; i < TABLE_SIZE; ++i) {
            int L = table[i].size();
            if (L > 0) {
                sumPositions += static_cast<long long>(L) * (L + 1) / 2;
            }
        }
        return static_cast<double>(sumPositions) / numElements;
    }

    // 打印当前哈希表状态（仅用于测试查看）
    void printTable() const {
        std::cout << "HashTable 状态 (共 " << TABLE_SIZE << " 桶):\n";
        for (int i = 0; i < TABLE_SIZE; ++i) {
            std::cout << "桶 " << i << " (长度 " << table[i].size() << "): ";
            if (table[i].empty()) {
                std::cout << "空\n";
                continue;
            }
            for (int x : table[i]) {
                std::cout << x << " ";
            }
            std::cout << "\n";
        }
        std::cout << "元素总数: " << numElements << "\n";
    }

private:
    int TABLE_SIZE;
    int numElements = 0;
    std::vector<std::list<int>> table;
};

int main() {
    // 示例：创建一个大小为 5 的哈希表
    HashTable ht(5);

    // 插入一些元素
    int keysToInsert[] = {10, 20, 15, 7, 12, 27, 32, 5, 18};
    std::cout << "插入元素: ";
    for (int k : keysToInsert) {
        std::cout << k << " ";
        ht.insert(k);
    }
    std::cout << "\n\n";

    // 打印哈希表的分布情况
    ht.printTable();
    std::cout << "\n";

    // 计算并打印 ASL1
    double asl1 = ht.ASL1();
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "成功检索情况下的平均检索长度 ASL1 = " << asl1 << "\n";

    return 0;
}

