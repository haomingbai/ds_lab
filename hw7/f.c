#include <stddef.h>
#include "./framework.c"

typedef struct char_node {
  char c;
  binary_node node;
} char_node;

// 获取层宽
void binary_node_get_level_width(binary_node *root, size_t curr_level,
                                 size_t widths[]) {
  // 空节点直接返回
  if (!root) {
    return;
  }

  // 非空节点代表宽度增加
  widths[curr_level]++;
  // 递归调用
  binary_node_get_level_width(root->child[LEFT], curr_level + 1, widths);
  binary_node_get_level_width(root->child[RIGHT], curr_level + 1, widths);
}

size_t binary_tree_get_max_width(binary_tree *tree) {
  size_t height = binary_tree_height(tree);
  size_t *w = calloc(height, sizeof(size_t));
  binary_node_get_level_width(tree->root, 0, w);
  size_t res = 0;
  for (size_t i = 0; i < height; i++) {
    if (w[i] > res) {
      res = w[i];
    }
  }
  free(w);
  return res;
}

// 辅助：创建一个 char_node，值为 c
char_node *make_node(char c) {
  char_node *p = malloc(sizeof(char_node));
  p->c = c;
  p->node.parent = NULL;
  p->node.child[LEFT] = p->node.child[RIGHT] = NULL;
  p->node.color = BLACK;
  return p;
}

int main(void) {
  binary_tree tree;
  init_binary_tree(&tree);

  // 构造示例二叉树：
  //
  //           'M'                     ← 第 1 层
  //          /   \
  //       'B'     'R'                 ← 第 2 层
  //      /  \      \
  //    'A'  'D'    'Z'                ← 第 3 层
  //         /
  //       'C'                         ← 第 4 层
  //
  // 期望最大宽度 = 3（第 3 层有 A, D, Z 共 3 个节点）

  char_node *nM = make_node('M');
  char_node *nB = make_node('B');
  char_node *nR = make_node('R');
  char_node *nA = make_node('A');
  char_node *nD = make_node('D');
  char_node *nZ = make_node('Z');
  char_node *nC = make_node('C');

  // 建立指针关系
  tree.root = &nM->node;
  tree.size = 7;

  nM->node.child[LEFT] = &nB->node;
  nM->node.child[RIGHT] = &nR->node;
  nB->node.parent = nR->node.parent = &nM->node;

  nB->node.child[LEFT] = &nA->node;
  nB->node.child[RIGHT] = &nD->node;
  nA->node.parent = nD->node.parent = &nB->node;

  nR->node.child[RIGHT] = &nZ->node;
  nZ->node.parent = &nR->node;

  nD->node.child[LEFT] = &nC->node;
  nC->node.parent = &nD->node;

  // 调用待测函数并打印
  size_t width = binary_tree_get_max_width(&tree);
  printf("Computed max width = %zu  (expected 3)\n", width);

  // 释放内存
  free(nM);
  free(nB);
  free(nR);
  free(nA);
  free(nD);
  free(nZ);
  free(nC);

  return 0;
}
