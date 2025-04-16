#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __GNUC__
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif

#define CONTAINER_OF(NODE_TYPE, MEMBER, NODE_PTR) \
  ((NODE_TYPE *)((char *)(NODE_PTR) - (offsetof(NODE_TYPE, MEMBER))))

typedef struct binary_node binary_node;
typedef binary_node binary_node;

enum direction { LEFT = 0, RIGHT = 1 };

struct binary_node {
  binary_node *parent;
  binary_node *child[2];
  int hight_diff;
};

enum traverse_order { PRE, IN, POST };

typedef struct binary_tree {
  binary_node *root;
  size_t size;
  size_t height;
} binary_tree;

typedef binary_tree bTree;

int init_binary_tree(binary_tree *tree) {
  tree->size = 0, tree->root = NULL;
  return 0;
}

binary_tree *make_binary_tree() {
  bTree *tree = (bTree *)malloc(sizeof(bTree));
#ifdef __GNUC__
  if (unlikely(!tree)) {
#else
  if (!tree) {
#endif
    return NULL;
  }
  init_binary_tree(tree);
  return tree;
}

// Notice: this function will never update the size of the tree for it is not
// efficient enough to be integrated and in different cases there exists many of
// different occasions.
binary_node *binary_tree_insert(binary_tree *tree, binary_node *node,
                                binary_node *new_node, enum direction dir) {
#ifdef __GNUC__
  if (unlikely(tree == NULL || new_node == NULL)) {
#else
  if (tree == NULL || new_node == NULL) {
#endif
    return NULL;
#ifdef __GNUC__
  } else if (unlikely(node == NULL)) {
#else
  } else if (node == NULL) {
#endif
    binary_node *old_node = tree->root;
    tree->root = new_node;
    new_node->parent = NULL, new_node->child[LEFT] = NULL,
    new_node->child[RIGHT] = NULL;
    return old_node;
  } else {
    binary_node *old_node = node->child[dir];
    node->child[dir] = new_node;
    new_node->child[LEFT] = NULL;
    new_node->child[RIGHT] = NULL;
    new_node->parent = node;
    return old_node;
  }
}

void *alloc_with_offset(size_t size, size_t offset) {
  void *ptr = malloc(size);
#ifdef __GNUC__
  if (likely(ptr)) {
#else
  if (ptr) {
#endif
    return (void *)((char *)ptr + offset);
  } else {
    return NULL;
  }
}

#define MAKE_BINARY_NODE(NODE_TYPE, MEMBER)            \
  ((binary_node *)alloc_with_offset(sizeof(NODE_TYPE), \
                                    offsetof(NODE_TYPE, MEMBER)))

#define FREE_binary_node(NODE_TYPE, MEMBER, NODE_PTR) \
  (free(CONTAINER_OF(NODE_TYPE, MEMBER, NODE_PTR)))

void binary_node_traverse(binary_node *node, void (*func)(binary_node *),
                          enum traverse_order order) {
  switch (order) {
    case PRE:
      func(node);
      if (node->child[LEFT] != NULL) {
        func(node->child[LEFT]);
      }
      if (node->child[RIGHT] != NULL) {
        func(node->child[RIGHT]);
      }
      break;
    case POST:
      if (node->child[LEFT] != NULL) {
        func(node->child[LEFT]);
      }
      if (node->child[RIGHT] != NULL) {
        func(node->child[RIGHT]);
      }
      func(node);
      break;
    case IN:
      if (node->child[LEFT] != NULL) {
        func(node->child[LEFT]);
      }
      func(node);
      if (node->child[RIGHT] != NULL) {
        func(node->child[RIGHT]);
      }
      break;
  }
}

void binary_tree_traverse(binary_tree *tree, void (*func)(binary_node *),
                          enum traverse_order order) {
  if (tree->root == NULL) {
    return;
  } else {
    binary_node_traverse(tree->root, func, order);
  }
}

void binary_node_switch_order(binary_node *node) {
  binary_node *tmp = node->child[LEFT];
  node->child[LEFT] = node->child[RIGHT];
  node->child[RIGHT] = tmp;
}

void binary_tree_reverse(binary_tree *tree) {
  binary_tree_traverse(tree, binary_node_switch_order, POST);
}

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

size_t binary_node_height(binary_node *node) {
  if (node == NULL) {
    return 0;
  } else {
    return MAX(binary_node_height(node->child[LEFT]),
               binary_node_height(node->child[RIGHT])) +
           1;
  }
}

size_t binary_tree_update_height(binary_tree *tree) {
  size_t res = binary_node_height(tree->root);
  tree->height = res;
  return res;
}

size_t binary_tree_gen_height(binary_tree *tree) {
  size_t res = binary_node_height(tree->root);
  tree->height = res;
  return res;
}

size_t binary_tree_get_size(binary_tree *tree) { return tree->size; }

size_t binary_tree_get_height(binary_tree *tree) { return tree->height; }

size_t binary_node_get_size(binary_node *root) {
  if (root == NULL) {
    return 0;
  }
  return 1 + binary_node_get_size(root->child[LEFT]) +
         binary_node_get_size(root->child[RIGHT]);
}

size_t binary_tree_update_size(binary_tree *tree) {
  size_t res = binary_node_get_size(tree->root);
  tree->size = res;
  return res;
}

size_t binary_tree_gen_size(binary_tree *tree) {
  size_t res = binary_node_get_size(tree->root);
  return res;
}

int binary_tree_rotate(binary_tree *tree, binary_node *node,
                       enum direction dir) {
  if (node == NULL || tree == NULL) {
    return -1;
  }
  if (node->parent == NULL) {
    assert(node == tree->root);

    switch (dir) {
      case LEFT: {
        if (node->child[RIGHT] == NULL) {
          return -2;
        }

        binary_node *old_root = node, *new_root = node->child[RIGHT],
                    *child_to_swap = node->child[RIGHT]->child[LEFT];
        tree->root = new_root;

        new_root->child[LEFT] = old_root;
        old_root->child[RIGHT] = child_to_swap;

        if (child_to_swap != NULL) {
          child_to_swap->parent = old_root;
        }
        old_root->parent = new_root;
        new_root->parent = NULL;

        return 0;
      }
      case RIGHT: {
        if (node->child[LEFT] == NULL) {
          return -2;
        }

        binary_node *old_root = node, *new_root = node->child[LEFT],
                    *child_to_swap = node->child[LEFT]->child[RIGHT];
        tree->root = new_root;

        new_root->child[RIGHT] = old_root;
        old_root->child[LEFT] = child_to_swap;

        if (child_to_swap != NULL) {
          child_to_swap->parent = old_root;
        }
        old_root->parent = new_root;
        new_root->parent = NULL;

        return 0;
      }
      default: {
        return -3;
      }
    }
  }
  switch (dir) {
    case LEFT: {
      if (node->child[RIGHT] == NULL) {
        return -2;
      }
      binary_node *old_root = node, *new_root = node->child[RIGHT],
                  *child_to_swap = node->child[RIGHT]->child[LEFT];
      binary_node *parent = node->parent;

      assert(!(node == parent->child[LEFT] && node == parent->child[RIGHT]));
      if (node == parent->child[LEFT]) {
        parent->child[LEFT] = new_root;
      } else if (node == parent->child[RIGHT]) {
        parent->child[RIGHT] = new_root;
      } else {
        assert(0);
      }

      new_root->child[LEFT] = old_root;
      old_root->child[RIGHT] = child_to_swap;
      if (child_to_swap != NULL) {
        child_to_swap->parent = old_root;
      }
      old_root->parent = new_root;
      new_root->parent = parent;

      return 0;
    }
    case RIGHT: {
      if (node->child[LEFT] == NULL) {
        return -2;
      }
      binary_node *old_root = node, *new_root = node->child[LEFT],
                  *child_to_swap = node->child[LEFT]->child[RIGHT];
      binary_node *parent = node->parent;

      assert(!(node == parent->child[LEFT] && node == parent->child[RIGHT]));
      if (node == parent->child[LEFT]) {
        parent->child[LEFT] = new_root;
      } else if (node == parent->child[RIGHT]) {
        parent->child[RIGHT] = new_root;
      } else {
        assert(0);
      }

      new_root->child[RIGHT] = old_root;
      old_root->child[LEFT] = child_to_swap;
      if (child_to_swap != NULL) {
        child_to_swap->parent = old_root;
      }
      old_root->parent = new_root;
      new_root->parent = parent;

      return 0;
    }
    default: {
      return -3;
    }
  }
}

binary_node *binary_node_find_lowest_ancestor(binary_node *root,
                                              binary_node *node1,
                                              binary_node *node2) {
  if (root == NULL) {
    return NULL;
  } else if (root == node1 || root == node2) {
    return root;
  } else {
    binary_node *left = binary_node_find_lowest_ancestor(root->child[LEFT],
                                                         node1, node2),
                *right = binary_node_find_lowest_ancestor(root->child[RIGHT],
                                                          node1, node2);
    if (left && right) {
      return root;
    } else {
      if (left) {
        return left;
      } else if (right) {
        return right;
      } else {
        assert(0);
      }
    }
  }
}

binary_node *binary_tree_find_lowest_ancestor(binary_tree *tree,
                                              binary_node *node1,
                                              binary_node *node2) {
  assert(binary_tree_gen_size(tree) >= 2);
  return binary_node_find_lowest_ancestor(tree->root, node1, node2);
}

// BTree algorithmm test code

typedef struct char_binary_node {
  char dat;
  binary_node node;
} char_binary_node;

int main(void) {
  binary_tree *tree = make_binary_tree();
  binary_node *node = MAKE_BINARY_NODE(char_binary_node, node);
  CONTAINER_OF(char_binary_node, node, node)->dat = 'a';
  binary_tree_insert(tree, NULL, node, LEFT);
  binary_node *nNode = MAKE_BINARY_NODE(char_binary_node, node);
  CONTAINER_OF(char_binary_node, node, nNode)->dat = 'b';
  binary_tree_insert(tree, node, nNode, LEFT);
  nNode = MAKE_BINARY_NODE(char_binary_node, node);
  CONTAINER_OF(char_binary_node, node, nNode)->dat = 'c';
  binary_tree_insert(tree, node, nNode, RIGHT);
  printf("%lu, %lu\n", binary_tree_update_height(tree),
         binary_tree_update_size(tree));
}

// #include "./sequence_list.c"
//
// // This, together with the binary_tree_get_ancestors_from_top, is only for
// // learning use.
// // It is not practical at all.
// bool binary_node_find_ancestor(binary_node *curr,
//                                                   binary_node *target,
//                                                   sequence_list *lst) {
//   if (curr == target) {
//     return true;
//   }
//   if (curr == NULL) {
//     return false;
//   }
//   bool child_result[2];
//   child_result[LEFT] =
//       binary_node_find_ancestor(curr->child[LEFT], target, lst);
//   child_result[RIGHT] =
//       binary_node_find_ancestor(curr->child[RIGHT], target, lst);
//   if (child_result[RIGHT] || child_result[LEFT]) {
//     SEQUENCE_LIST_PUSH_BACK(binary_node *, lst, curr);
//     return true;
//   }
//   return false;
// }
//
// bool binary_tree_get_ancestors_from_top(binary_tree *tree,
//                                         binary_node *target,
//                                         sequence_list *lst) {
//   bool res = binary_node_find_ancestor(tree->root, target, lst);
//   return res;
// }
