#include "./framework.c"

// Construct a struct of the node.
typedef struct {
  char dat;
  binary_node node;
} cnode_t;

// 修复函数
void repairTreeNode(binary_node *root) {
  if (!root) {
    return;
  }

  // 修复子节点
  // 递归调用
  if (root->child[LEFT]) {
    root->child[LEFT]->parent = root;
    repairTreeNode(root->child[LEFT]);
  }
  if (root->child[RIGHT]) {
    root->child[RIGHT]->parent = root;
    repairTreeNode(root->child[RIGHT]);
  }
  return;
}

void test_repair_parent() {
  // 创建测试节点
  binary_node *nodes[5];
  for (int i = 0; i < 5; i++) {
    // 使用宏创建嵌入binary_node的节点
    nodes[i] = MAKE_BINARY_NODE(cnode_t, node);
    assert(nodes[i] && "Allocation failed");
  }

  // 获取容器指针并设置字母
  for (int i = 0; i < 5; i++) {
    cnode_t *cn = CONTAINER_OF(cnode_t, node, nodes[i]);
    cn->dat = 'A' + i;  // 设置测试数据
  }

  /* 构建测试树结构：
         A(nodes[0])
        /         \
      B(nodes[1])  C(nodes[2])
     /     \
 D(nodes[3]) E(nodes[4]) */
  nodes[0]->child[LEFT] = nodes[1];   // A->B
  nodes[0]->child[RIGHT] = nodes[2];  // A->C
  nodes[1]->child[LEFT] = nodes[3];   // B->D
  nodes[1]->child[RIGHT] = nodes[4];  // B->E

  // 执行修复操作
  repairTreeNode(nodes[0]);

  // 验证结果
  assert(nodes[0]->parent == NULL);      // root
  assert(nodes[1]->parent == nodes[0]);  // B's parent is A
  assert(nodes[2]->parent == nodes[0]);  // C's parent is A
  assert(nodes[3]->parent == nodes[1]);  // D's parent is B
  assert(nodes[4]->parent == nodes[1]);  // E's parent is B

  // 释放内存（必须使用容器指针）
  for (int i = 0; i < 5; i++) {
    cnode_t *cn = CONTAINER_OF(cnode_t, node, nodes[i]);
    free(cn);
  }

  printf("Parent pointer test passed!\n");
}

int main() {
  test_repair_parent();
  return 0;
}
