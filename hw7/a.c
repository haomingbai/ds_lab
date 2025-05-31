#include "framework.c"

// Construct a struct of the node.
typedef struct {
  char dat;
  binary_node node;
} cnode_t;

// 计算单分支节点个树, 利用遍历方案
size_t binary_node_cnt_single_node_num(binary_node *root) {
  if (!root) {
    return 0;
  }
  if (root->child[LEFT] && root->child[RIGHT]) {
    return binary_node_cnt_single_node_num(root->child[LEFT]) +
           binary_node_cnt_single_node_num(root->child[RIGHT]);
  }
  // 检测到单分支节点, 计数+1
  else if (root->child[LEFT]) {
    return 1 + binary_node_cnt_single_node_num(root->child[LEFT]);
  } else if (root->child[RIGHT]) {
    return 1 + binary_node_cnt_single_node_num(root->child[RIGHT]);
  } else {
    return 0;
  }
}

// 对于二叉树的函数
size_t binary_tree_cnt_single_node_num(binary_tree *tree) {
  return binary_node_cnt_single_node_num(tree->root);
}

// 测试用代码, 本地二叉树构造函数
void construct_btree_local(binary_tree *tree) {
  init_binary_tree(tree);
  binary_node *root = MAKE_BINARY_NODE(cnode_t, node);
  binary_tree_insert(tree, NULL, root, LEFT);
  __auto_type it = root;
  for (size_t i = 0; i < 10; i++) {
    __auto_type node = MAKE_BINARY_NODE(cnode_t, node);
    binary_tree_insert(tree, it, node, LEFT);
    it = node;
  }
  it = root;
  for (size_t i = 0; i < 10; i++) {
    __auto_type node = MAKE_BINARY_NODE(cnode_t, node);
    binary_tree_insert(tree, it, node, RIGHT);
    it = node;
  }
}

int main(void) {
  binary_tree tree;
  construct_btree_local(&tree);
  printf("%lu\n", binary_tree_cnt_single_node_num(&tree));
}
