#include <stdbool.h>

typedef struct binary_tree_node {
#define bNode struct binary_tree_node
  bNode *parent;
  bNode *left_child, *right_child;
#undef bNode
} binary_tree_node;

typedef struct bool_node {
  bool dat;
  binary_tree_node node;
} bool_node;
