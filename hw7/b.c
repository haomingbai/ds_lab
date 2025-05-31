#include "./framework.c"

// Construct a struct of the node.
typedef struct {
  char dat;
  binary_node node;
} cnode_t;

size_t binary_node_cnt_leave_node_num(binary_node *root) {
  if (!root) {
    return 0;
  }
  // 叶子返回
  else if (!root->child[LEFT] && !root->child[RIGHT]) {
    return 1;
  }
  size_t res = 0;

  // 遍历树
  if (root->child[LEFT]) {
    res += binary_node_cnt_leave_node_num(root->child[LEFT]);
  }
  if (root->child[RIGHT]) {
    res += binary_node_cnt_leave_node_num(root->child[RIGHT]);
  }
  return res;
}

size_t binary_tree_cnt_leave_node_num(binary_tree *tree) {
  return binary_node_cnt_leave_node_num(tree->root);
}

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
  printf("%lu\n", binary_tree_cnt_leave_node_num(&tree));
}
