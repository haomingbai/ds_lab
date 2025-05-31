#include "./framework.c"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

typedef struct char_node {
  char c;
  binary_node node;
} char_node;

// 计算最小层次, 复杂度O(n)
size_t find_level(binary_node *root, char c) {
  if (root == NULL) {
    return -1;
  }
  if (CONTAINER_OF(char_node, node, root)->c == c) {
    return 1;
  } else {
    // 递归查找
    size_t res = MIN(find_level(root->child[LEFT], c),
                     find_level(root->child[RIGHT], c));
    // 未找到:
    if (res == -1) {
      return -1;
    } else {
      // 结果传递
      return res + 1;
    }
  }
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
  // 构造如上示例树
  char_node *n1 = make_node('a');
  char_node *n2 = make_node('b');
  char_node *n3 = make_node('a');
  char_node *n4 = make_node('a');
  char_node *n5 = make_node('c');

  // 建立指针关系
  //       n1
  //      /  \
  //    n2    n3
  //    /      \
  //  n4        n5
  n1->node.child[LEFT]  = &n2->node;
  n1->node.child[RIGHT] = &n3->node;
  n2->node.parent      = &n1->node;
  n3->node.parent      = &n1->node;

  n2->node.child[LEFT] = &n4->node;
  n4->node.parent     = &n2->node;

  n3->node.child[RIGHT] = &n5->node;
  n5->node.parent      = &n3->node;

  // 测试
  printf("find_level('a') = %zd  (expected 1)\n", find_level(&n1->node, 'a'));
  printf("find_level('b') = %zd  (expected 2)\n", find_level(&n1->node, 'b'));
  printf("find_level('c') = %zd  (expected 3)\n", find_level(&n1->node, 'c'));
  printf("find_level('x') = %zd  (expected -1)\n", find_level(&n1->node, 'x'));

  // 释放
  free(n1); free(n2); free(n3); free(n4); free(n5);
  return 0;
}
