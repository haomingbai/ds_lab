#include "./framework.c"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

typedef struct char_node {
  char c;
  binary_node node;
} char_node;

void print_char_node(binary_node *node) {
  printf("%c ", CONTAINER_OF(char_node, node, node)->c);
}

// 找到X, 遍历其子孙
void binary_node_find_char(binary_node *root, char c, void(*func)(binary_node *)) {
  if (!root) {
    return;
  }
  if (CONTAINER_OF(char_node, node, root)->c == c) {
    binary_node_traverse(root, func, PRE);
  }
  binary_node_find_char(root->child[LEFT], c, func);
  binary_node_find_char(root->child[RIGHT], c, func);
}


// 辅助：创建一个 char_node，值为 c
char_node* make_node(char c) {
  char_node *p = malloc(sizeof(char_node));
  p->c = c;
  p->node.parent = NULL;
  p->node.child[LEFT] = p->node.child[RIGHT] = NULL;
  p->node.color = BLACK;
  return p;
}

int main(void) {
  // 构造示例二叉树：
  //
  //           'M'
  //          /   \
  //        'B'   'R'
  //       /   \     \
  //     'A'   'D'   'Z'
  //           / \
  //         'C' 'E'
  //
  char_node *nM = make_node('M');
  char_node *nB = make_node('B');
  char_node *nR = make_node('R');
  char_node *nA = make_node('A');
  char_node *nD = make_node('D');
  char_node *nC = make_node('C');
  char_node *nE = make_node('E');
  char_node *nZ = make_node('Z');

  // 连接关系
  nM->node.child[LEFT]  = &nB->node;
  nM->node.child[RIGHT] = &nR->node;
  nB->node.parent = nR->node.parent = &nM->node;

  nB->node.child[LEFT]  = &nA->node;
  nB->node.child[RIGHT] = &nD->node;
  nA->node.parent = nD->node.parent = &nB->node;

  nD->node.child[LEFT]  = &nC->node;
  nD->node.child[RIGHT] = &nE->node;
  nC->node.parent = nE->node.parent = &nD->node;

  nR->node.child[RIGHT] = &nZ->node;
  nZ->node.parent      = &nR->node;

  // 测试 1：x = 'D'
  printf("Subtree of 'D': ");
  binary_node_find_char(&nM->node, 'D', print_char_node);
  printf("\nExpected: D C E\n\n");

  // 测试 2：x = 'B'
  printf("Subtree of 'B': ");
  binary_node_find_char(&nM->node, 'B', print_char_node);
  printf("\nExpected: B A D C E\n\n");

  // 测试 3：x = 'X'（不存在）
  printf("Subtree of 'X': ");
  binary_node_find_char(&nM->node, 'X', print_char_node);
  printf("\nExpected: (no output or a not-found indication)\n");

  // 释放内存
  free(nM); free(nB); free(nR);
  free(nA); free(nD); free(nC); free(nE); free(nZ);
  return 0;
}
