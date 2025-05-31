#include <stdbool.h>
#include "./framework.c"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

typedef struct char_node {
  char c;
  binary_node node;
} char_node;

int __is_cbn(binary_node *root) {
  if (root == NULL) {
    return 0;
  }
  int left_height = __is_cbn(root->child[LEFT]);
  int right_height = __is_cbn(root->child[RIGHT]);

  // 不是完全二叉树返回-1
  if (left_height == -1 || right_height == -1) {
    return -1;
  }
  // 高度差正确返回树高
  if (left_height - right_height == 0 || left_height - right_height == 1) {
    return left_height + 1;
  }
  // 高度不对返回-1
  return -1;
}

bool is_complete_binary_node(binary_node *root) {
  int r = __is_cbn(root);
  if (r != -1) {
    return true;
  }
  return false;
}

bool is_complete_binary_tree(binary_tree *tree) {
  return is_complete_binary_node(tree->root);
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

// 释放全树
void free_subtree(binary_node *n) {
  if (!n) return;
  free_subtree(n->child[LEFT]);
  free_subtree(n->child[RIGHT]);
  free(CONTAINER_OF(char_node, node, n));
}

void run_test(const char *name, binary_tree *t, bool expected) {
  bool result = is_complete_binary_tree(t);
  printf("%s: result = %s  (expected %s)\n",
         name,
         result ? "true" : "false",
         expected ? "true" : "false");
}

int main(void) {
  // 测试1：空树——应为完全二叉树
  binary_tree t1 = { .root = NULL, .size = 0 };
  run_test("Test1 Empty", &t1, true);

  // 测试2：只有根节点——完全二叉树
  {
    binary_tree t2;
    char_node *n = make_node('A');
    t2.root = &n->node;
    t2.size = 1;
    run_test("Test2 Single Node", &t2, true);
    free(n);
  }

  // 测试3：一棵完美二叉树（满二叉树）
  //        'A'
  //       /   \
  //     'B'   'C'
  //    / \    / \
  //  'D' 'E''F' 'G'
  {
    binary_tree t3;
    char_node *nA = make_node('A');
    char_node *nB = make_node('B');
    char_node *nC = make_node('C');
    char_node *nD = make_node('D');
    char_node *nE = make_node('E');
    char_node *nF = make_node('F');
    char_node *nG = make_node('G');
    t3.root = &nA->node;
    t3.size = 7;
    nA->node.child[LEFT]  = &nB->node; nB->node.parent = &nA->node;
    nA->node.child[RIGHT] = &nC->node; nC->node.parent = &nA->node;
    nB->node.child[LEFT]  = &nD->node; nD->node.parent = &nB->node;
    nB->node.child[RIGHT] = &nE->node; nE->node.parent = &nB->node;
    nC->node.child[LEFT]  = &nF->node; nF->node.parent = &nC->node;
    nC->node.child[RIGHT] = &nG->node; nG->node.parent = &nC->node;
    run_test("Test3 Perfect Tree", &t3, true);
    free_subtree(&nA->node);
  }

  // 测试4：完全但不满的二叉树
  //        'A'
  //       /   \
  //     'B'   'C'
  //    /  \
  //  'D'  'E'
  {
    binary_tree t4;
    char_node *nA = make_node('A');
    char_node *nB = make_node('B');
    char_node *nC = make_node('C');
    char_node *nD = make_node('D');
    char_node *nE = make_node('E');
    t4.root = &nA->node;
    t4.size = 5;
    nA->node.child[LEFT]  = &nB->node; nB->node.parent = &nA->node;
    nA->node.child[RIGHT] = &nC->node; nC->node.parent = &nA->node;
    nB->node.child[LEFT]  = &nD->node; nD->node.parent = &nB->node;
    nB->node.child[RIGHT] = &nE->node; nE->node.parent = &nB->node;
    run_test("Test4 Complete Not Perfect", &t4, true);
    free_subtree(&nA->node);
  }

  // 测试5：非完全二叉树——左子树缺失右子节点，而右子树存在
  //        'A'
  //       /   \
  //     'B'   'C'
  //      \      \
  //      'E'    'F'
  {
    binary_tree t5;
    char_node *nA = make_node('A');
    char_node *nB = make_node('B');
    char_node *nC = make_node('C');
    char_node *nE = make_node('E');
    char_node *nF = make_node('F');
    t5.root = &nA->node;
    t5.size = 5;
    nA->node.child[LEFT]  = &nB->node; nB->node.parent = &nA->node;
    nA->node.child[RIGHT] = &nC->node; nC->node.parent = &nA->node;
    nB->node.child[RIGHT] = &nE->node; nE->node.parent = &nB->node;
    nC->node.child[RIGHT] = &nF->node; nF->node.parent = &nC->node;
    run_test("Test5 Not Complete", &t5, false);
    free_subtree(&nA->node);
  }

  return 0;
}
