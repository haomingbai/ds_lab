#include <iostream>
#include <vector>
#include <list>

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
        // 检查是否已存在，若不存在则插入
        for (int x : table[idx]) {
            if (x == key) return;
        }
        table[idx].push_back(key);
        ++numElements;
    }

    // 删除关键字为 k 的元素（若不存在则什么都不做）
    void remove(int key) {
        int idx = hashFunc(key);
        auto& chain = table[idx];
        for (auto it = chain.begin(); it != chain.end(); ++it) {
            if (*it == key) {
                chain.erase(it);
                --numElements;
                return;
            }
        }
        // 若遍历完未找到，则不做任何操作
    }

    // 判断 k 是否在哈希表中
    bool contains(int key) const {
        int idx = hashFunc(key);
        for (int x : table[idx]) {
            if (x == key) return true;
        }
        return false;
    }

    // 打印当前哈希表状态（每个桶里的链表）
    void printTable() const {
        std::cout << "HashTable 状态 (共 " << TABLE_SIZE << " 桶):\n";
        for (int i = 0; i < TABLE_SIZE; ++i) {
            std::cout << "桶 " << i << ": ";
            if (table[i].empty()) {
                std::cout << "空\n";
                continue;
            }
            for (int x : table[i]) {
                std::cout << x << " -> ";
            }
            std::cout << "NULL\n";
        }
        std::cout << "当前元素总数: " << numElements << "\n\n";
    }

private:
    int TABLE_SIZE;
    int numElements = 0;
    std::vector<std::list<int>> table;
};

int main() {
    // 示例：创建大小为 7 的哈希表
    HashTable ht(7);

    // 插入一些元素
    int keysToInsert[] = {15, 11, 27, 8, 12, 5, 14};
    std::cout << "插入元素: ";
    for (int k : keysToInsert) {
        std::cout << k << " ";
        ht.insert(k);
    }
    std::cout << "\n\n";

    // 打印插入后状态
    std::cout << "插入后:\n";
    ht.printTable();

    // 测试删除：先删除一些存在的，再尝试删除不存在的
    int keysToRemove[] = {27, 5, 100, 12};
    for (int k : keysToRemove) {
        std::cout << "尝试删除 " << k << " ... ";
        if (ht.contains(k)) {
            ht.remove(k);
            std::cout << "已删除。\n";
        } else {
            std::cout << "表中不存在，跳过。\n";
        }
    }
    std::cout << "\n";

    // 删除后再打印一次
    std::cout << "删除后:\n";
    ht.printTable();

    return 0;
}

