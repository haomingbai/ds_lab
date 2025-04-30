#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
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
enum color { RED = 0, BLACK = 1 };

struct binary_node {
  binary_node *parent;
  binary_node *child[2];
  enum color color;
};

enum traverse_order { PRE, IN, POST };

typedef struct binary_tree {
  binary_node *root;
  size_t size;
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
// Notice: make sure that the node orphan node.
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

binary_node *binary_tree_transplant(binary_tree *tree, binary_node *old_node,
                                    binary_node *new_node) {
  binary_node *parent = old_node->parent;
  if (new_node != NULL) {
    new_node->parent = parent;
  }
  if (parent == NULL) {
    assert(tree->root == old_node);
    tree->root = new_node;
  } else {
    assert(tree->root != old_node);
    if (parent->child[LEFT] == old_node) {
      parent->child[LEFT] = new_node;
    } else if (parent->child[RIGHT] == old_node) {
      parent->child[RIGHT] = new_node;
    } else {
      assert(0);
    }
  }
  return old_node;
}

void *__alloc_with_offset(size_t size, size_t offset) {
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

#define MAKE_BINARY_NODE(NODE_TYPE, MEMBER)              \
  ((binary_node *)__alloc_with_offset(sizeof(NODE_TYPE), \
                                      offsetof(NODE_TYPE, MEMBER)))

#define FREE_BINARY_NODE(NODE_TYPE, MEMBER, NODE_PTR) \
  (free(CONTAINER_OF(NODE_TYPE, MEMBER, NODE_PTR)))

void binary_node_switch_order(binary_node *node) {
  binary_node *tmp = node->child[LEFT];
  node->child[LEFT] = node->child[RIGHT];
  node->child[RIGHT] = tmp;
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

size_t binary_tree_height(binary_tree *tree) {
  size_t res = binary_node_height(tree->root);
  return res;
}

size_t binary_tree_get_size(binary_tree *tree) { return tree->size; }

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

void __release_with_offset(void *ptr, size_t offset) {
  if (ptr == NULL) {
    return;
  }
  free((void *)((char *)ptr - offset));
}

void __free_binary_root_with_offset(binary_node *root, size_t offset) {
  if (root == NULL) {
    return;
  }
  __free_binary_root_with_offset(root->child[LEFT], offset);
  __free_binary_root_with_offset(root->child[RIGHT], offset);
  __release_with_offset(root, offset);
}

#define DESTROY_BINARY_TREE(NODE_TYPE, MEMBER, TREE_PTR) \
  __free_binary_root_with_offset(((TREE_PTR)->root),     \
                                 (offsetof(NODE_TYPE, MEMBER)))

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

void binary_node_traverse(binary_node *node, void (*func)(binary_node *),
                          enum traverse_order order) {
  switch (order) {
    case PRE:
      func(node);
      if (node->child[LEFT] != NULL) {
        binary_node_traverse(node->child[LEFT], func, order);
      }
      if (node->child[RIGHT] != NULL) {
        binary_node_traverse(node->child[RIGHT], func, order);
      }
      break;
    case POST:
      if (node->child[LEFT] != NULL) {
        binary_node_traverse(node->child[LEFT], func, order);
      }
      if (node->child[RIGHT] != NULL) {
        binary_node_traverse(node->child[RIGHT], func, order);
      }
      func(node);
      break;
    case IN:
      if (node->child[LEFT] != NULL) {
        binary_node_traverse(node->child[LEFT], func, order);
      }
      func(node);
      if (node->child[RIGHT] != NULL) {
        binary_node_traverse(node->child[RIGHT], func, order);
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

typedef binary_tree bst;
typedef binary_node bst_node;

int init_bst(bst *tree) { return init_binary_tree(tree); }

int bst_insert(bst *tree, bst_node *node,
               int (*cmp)(const bst_node *, const bst_node *)) {
  assert(node);
  if (binary_tree_get_size(tree) == 0) {
    binary_tree_insert(tree, NULL, node, LEFT);
    tree->size++;
    return 0;
  }

  bst_node *curr = tree->root;
  while (true) {
    assert(curr);
    int cmp_res = cmp(curr, node);
    if (!cmp_res) {
      return -1;
    }
    if (cmp_res > 0) {
      if (curr->child[LEFT] == NULL) {
        binary_tree_insert(tree, curr, node, LEFT);
        tree->size++;
        return 0;
      } else {
        curr = curr->child[LEFT];
      }
    } else {
      if (curr->child[RIGHT] == NULL) {
        binary_tree_insert(tree, curr, node, RIGHT);
        tree->size++;
        return 0;
      } else {
        curr = curr->child[RIGHT];
      }
    }
  }
}

bst_node *bst_find(bst *tree, bst_node *node,
                   int (*cmp)(const bst_node *, const bst_node *)) {
  if (!binary_tree_get_size(tree)) {
    return NULL;
  }
  if (!node) {
    return NULL;
  }

  bst_node *curr = tree->root;
  while (curr) {
    int cmp_res = cmp(curr, node);
    if (!cmp_res) {
      return curr;
    } else if (cmp_res < 0) {
      curr = curr->child[RIGHT];
    } else {
      curr = curr->child[LEFT];
    }
  }
  return curr;
}

// Be careful to use
bst_node *bst_remove(bst *tree, bst_node *node,
                     int (*cmp)(const bst_node *, const bst_node *)) {
  bst_node *node_to_remove = bst_find(tree, node, cmp);
  if (node_to_remove == NULL) {
    return NULL;
  }
  tree->size--;
  // The parent == null need to be concerned!
  if (node_to_remove->child[LEFT] == NULL &&
      node_to_remove->child[RIGHT] == NULL) {
    bst_node *parent = node_to_remove->parent;
    if (parent == NULL) {
      tree->root = NULL;
      return node_to_remove;
    } else if (node_to_remove == parent->child[LEFT]) {
      parent->child[LEFT] = NULL;
      node_to_remove->parent = NULL;
      node_to_remove->child[LEFT] = NULL;
      node_to_remove->child[RIGHT] = NULL;
      return node_to_remove;
    } else if (node_to_remove == parent->child[RIGHT]) {
      parent->child[RIGHT] = NULL;
      node_to_remove->parent = NULL;
      node_to_remove->child[LEFT] = NULL;
      node_to_remove->child[RIGHT] = NULL;
      return node_to_remove;
    } else {
      assert(0);
    }
  } else if (node_to_remove->child[LEFT] == NULL) {
    bst_node *parent = node_to_remove->parent;
    if (parent == NULL) {
      tree->root = node_to_remove->child[RIGHT];
      node_to_remove->child[RIGHT]->parent = NULL;
      node_to_remove->child[RIGHT] = NULL;
      return node_to_remove;
    } else if (parent->child[LEFT] == node_to_remove) {
      parent->child[LEFT] = node_to_remove->child[RIGHT];
      node_to_remove->child[RIGHT]->parent = parent;
      node_to_remove->parent = NULL;
      node_to_remove->child[LEFT] = NULL;
      node_to_remove->child[RIGHT] = NULL;
      return node_to_remove;
    } else if (parent->child[RIGHT] == node_to_remove) {
      parent->child[RIGHT] = node_to_remove->child[RIGHT];
      node_to_remove->child[RIGHT]->parent = parent;
      node_to_remove->parent = NULL;
      node_to_remove->child[LEFT] = NULL;
      node_to_remove->child[RIGHT] = NULL;
      return node_to_remove;
    } else {
      assert(0);
    }
  } else if (node_to_remove->child[RIGHT] == NULL) {
    bst_node *parent = node_to_remove->parent;
    if (parent == NULL) {
      tree->root = node_to_remove->child[LEFT];
      node_to_remove->child[LEFT]->parent = NULL;
      node_to_remove->child[LEFT] = NULL;
      return node_to_remove;
    } else if (parent->child[LEFT] == node_to_remove) {
      parent->child[LEFT] = node_to_remove->child[LEFT];
      node_to_remove->child[LEFT]->parent = parent;
      node_to_remove->parent = NULL;
      node_to_remove->child[LEFT] = NULL;
      node_to_remove->child[RIGHT] = NULL;
      return node_to_remove;
    } else if (parent->child[RIGHT] == node_to_remove) {
      parent->child[RIGHT] = node_to_remove->child[LEFT];
      node_to_remove->child[LEFT]->parent = parent;
      node_to_remove->parent = NULL;
      node_to_remove->child[LEFT] = NULL;
      node_to_remove->child[RIGHT] = NULL;
      return node_to_remove;
    } else {
      assert(0);
    }
  } else {
    bst_node *nNode = node_to_remove->child[RIGHT];
    while (nNode->child[LEFT]) {
      nNode = nNode->child[LEFT];
    }
    if (nNode->child[LEFT] == NULL && nNode->child[RIGHT] == NULL) {
      bst_node *parent = nNode->parent;
      if (nNode == parent->child[LEFT]) {
        parent->child[LEFT] = NULL;
        nNode->parent = NULL;
      } else if (nNode == parent->child[RIGHT]) {
        parent->child[RIGHT] = NULL;
        nNode->parent = NULL;
      } else {
        assert(0);
      }
    } else {
      bst_node *parent = nNode->parent;
      bst_node *child = nNode->child[RIGHT];
      child->parent = parent;
      if (parent->child[LEFT] == nNode) {
        parent->child[LEFT] = nNode->child[RIGHT];
        nNode->parent = NULL;
      } else if (parent->child[RIGHT] == nNode) {
        parent->child[RIGHT] = nNode->child[RIGHT];
        nNode->parent = NULL;
      } else {
        assert(0);
      }
    }
    nNode->parent = node_to_remove->parent;
    nNode->child[LEFT] = node_to_remove->child[LEFT];
    nNode->child[RIGHT] = node_to_remove->child[RIGHT];
    nNode->child[LEFT]->parent = nNode;
    nNode->child[RIGHT]->parent = nNode;
    bst_node *parent = node_to_remove->parent;
    if (parent == NULL) {
      tree->root = nNode;
      nNode->parent = NULL;
    } else if (parent->child[LEFT] == node_to_remove) {
      parent->child[LEFT] = nNode;
    } else if (parent->child[RIGHT] == node_to_remove) {
      parent->child[RIGHT] = nNode;
    } else {
      assert(0);
    }
    node_to_remove->child[LEFT] = NULL;
    node_to_remove->child[RIGHT] = NULL;
    node_to_remove->parent = NULL;
    return node_to_remove;
  }
}

binary_node *bst_get_smallest(binary_tree *tree) {
  binary_node *it = tree->root;
  if (!it) {
    return it;
  }
  while (it->child[LEFT]) {
    it = it->child[LEFT];
  }
  return it;
}

binary_node *bst_get_greatest(binary_tree *tree) {
  binary_node *it = tree->root;
  if (!it) {
    return it;
  }
  while (it->child[RIGHT]) {
    it = it->child[RIGHT];
  }
  return it;
}

// RB Tree

typedef binary_tree rb_tree;

int init_rb_tree(rb_tree *tree) { return init_binary_tree(tree); }

static inline binary_node *_rbTree_get_grandpa(binary_node *node) {
  binary_node *parent = node->parent;
  if (parent == NULL) {
    return parent;
  }
  binary_node *grandpa = parent->parent;
  return grandpa;
}

static inline binary_node *_rbTree_get_uncle(binary_node *node) {
  binary_node *parent = node->parent;
  if (parent == NULL) {
    return parent;
  }
  binary_node *grandpa = parent->parent;
  if (grandpa == NULL) {
    return NULL;
  }
  if (parent == grandpa->child[LEFT]) {
    return grandpa->child[RIGHT];
  } else if (parent == grandpa->child[RIGHT]) {
    return grandpa->child[LEFT];
  } else {
    assert(0);
  }
}

static inline enum color _rbTree_color(binary_node *node) {
  if (node == NULL) {
    return BLACK;
  } else {
    return node->color;
  }
}

static inline void _rbTree_set_insert_balance(rb_tree *tree,
                                              binary_node *node) {
  // Maintain the balance.
  if (node == tree->root) {
    node->color = BLACK;
  } else if (_rbTree_color(node->parent) == RED &&
             _rbTree_color(_rbTree_get_uncle(node)) == RED) {
    binary_node *parent = node->parent, *uncle = _rbTree_get_uncle(node),
                *grandpa = _rbTree_get_grandpa(node);
    parent->color = BLACK, uncle->color = BLACK, grandpa->color = RED;
    _rbTree_set_insert_balance(tree, grandpa);
  } else if (_rbTree_color(node->parent) == RED &&
             _rbTree_color(_rbTree_get_uncle(node)) == BLACK) {
    binary_node *parent = node->parent, *uncle = _rbTree_get_uncle(node),
                *grandpa = _rbTree_get_grandpa(node);
    if (parent == grandpa->child[LEFT] && node == parent->child[RIGHT]) {
      binary_tree_rotate(tree, parent, LEFT);
      binary_node *mid = node;
      node = parent;
      parent = mid;
    } else if (parent == grandpa->child[RIGHT] && node == parent->child[LEFT]) {
      binary_tree_rotate(tree, parent, RIGHT);
      binary_node *mid = node;
      node = parent;
      parent = mid;
    }

    if (parent == grandpa->child[LEFT] && node == parent->child[LEFT]) {
      binary_tree_rotate(tree, grandpa, RIGHT);
      // At this time, parent on the root, grandpa on the right parent and node
      // on the parent.
      parent->color = BLACK;
      node->color = RED;
      grandpa->color = RED;
    } else if (parent == grandpa->child[RIGHT] &&
               node == parent->child[RIGHT]) {
      binary_tree_rotate(tree, grandpa, LEFT);
      parent->color = BLACK;
      node->color = RED;
      grandpa->color = RED;
    }
  }

  if (tree->root) {
    tree->root->color = BLACK;
  }
}

int rb_tree_insert(rb_tree *tree, binary_node *node,
                   int (*cmp)(const binary_node *, const binary_node *)) {
  int insert_res = bst_insert(tree, node, cmp);
  if (insert_res) {
    return insert_res;
  }

  node->color = RED;
  _rbTree_set_insert_balance(tree, node);
  return 0;
}

static inline binary_node *_findSuccessor(binary_node *root) {
  binary_node *node = root->child[RIGHT];
  while (node && node->child[LEFT]) {
    node = node->child[LEFT];
  }
  return node;
}

static inline binary_node *_rbTreeFindNode(rb_tree *tree, binary_node *node,
                                           int (*cmp)(const binary_node *,
                                                      const binary_node *)) {
  if (!tree->root) {
    return NULL;
  }

  binary_node *it = tree->root;
  int c = cmp(it, node);
  while (c) {
    if (c > 0) {
      it = it->child[LEFT];
    } else if (c < 0) {
      it = it->child[RIGHT];
    }
    if (!it) {
      break;
    }
    c = cmp(it, node);
  }
  return it;
}

binary_node *rb_tree_find_node(rb_tree *tree, binary_node *node,
                               int (*cmp)(const binary_node *,
                                          const binary_node *)) {
  return _rbTreeFindNode(tree, node, cmp);
}

static inline void _rbTree_switch_color(binary_node *node1,
                                        binary_node *node2) {
  enum color mid;
  if (node2 == node1) {
    return;
  }
  mid = node1->color;
  node1->color = node2->color;
  node2->color = mid;
}

// To be realized.
// Direction is the direction of the removed node.
static inline void _rbTreeRemoveMaintain(rb_tree *tree, binary_node *parent,
                                         enum direction dir) {
  binary_node *bro = parent->child[dir == LEFT ? RIGHT : LEFT];
  binary_node *close_nephew = NULL, *dist_nephew = NULL;
  if (bro) {
    close_nephew = bro->child[dir],
    dist_nephew = bro->child[dir == LEFT ? RIGHT : LEFT];
  }

  // 5 cases.
  // Case 1: color of bro is red.
  if (_rbTree_color(bro) == RED) {
    assert(parent->color == BLACK);
    binary_tree_rotate(tree, parent, dir);
    parent->color = RED;
    bro->color = BLACK;
  }
  // Case 2: color of brother and both nephews are black while parent is black
  else if (_rbTree_color(bro) == BLACK &&
           _rbTree_color(close_nephew) == BLACK &&
           _rbTree_color(dist_nephew) == BLACK &&
           _rbTree_color(parent) == RED) {
    bro->color = RED;
    parent->color = BLACK;
  }
  // Case 3: color of brother, both nephews and parent are all black.
  else if (_rbTree_color(bro) == BLACK &&
           _rbTree_color(close_nephew) == BLACK &&
           _rbTree_color(dist_nephew) == BLACK &&
           _rbTree_color(parent) == BLACK) {
    bro->color = RED;
  }
  // Case 4:
  else if (_rbTree_color(bro) == BLACK && _rbTree_color(close_nephew) == RED &&
           _rbTree_color(dist_nephew) == BLACK) {
    enum direction rotate_dir = (dir == LEFT ? RIGHT : LEFT);
    binary_tree_rotate(tree, bro, rotate_dir);
    close_nephew->color = BLACK;
    bro->color = RED;
    dist_nephew = bro;
    bro = close_nephew;
    close_nephew = bro->child[dir];
    goto start_case_5;
  }
  // Case 5:
  else if (_rbTree_color(bro) == BLACK && _rbTree_color(dist_nephew) == RED) {
  start_case_5:
    binary_tree_rotate(tree, parent, dir);
    dist_nephew->color = BLACK;
    _rbTree_switch_color(parent, bro);
  }
}

binary_node *rb_tree_remove(rb_tree *tree, binary_node *node,
                            int (*cmp)(const binary_node *,
                                       const binary_node *)) {
  binary_node *to_remove = _rbTreeFindNode(tree, node, cmp);
  if (!to_remove) {
    return NULL;
  }

  // 4 cases
  tree->size--;
  // Case 0: node is root
  if (to_remove == tree->root && to_remove->child[LEFT] == NULL &&
      to_remove->child[RIGHT] == NULL) {
    tree->root = NULL;
  }
  // Case 1: node is leaf
  else if (!to_remove->child[LEFT] && !to_remove->child[RIGHT]) {
    binary_node *parent = to_remove->parent;
    enum direction dir;
    if (to_remove == parent->child[LEFT]) {
      parent->child[LEFT] = NULL;
      dir = LEFT;
    } else if (to_remove == parent->child[RIGHT]) {
      parent->child[RIGHT] = NULL;
      dir = RIGHT;
    }

    // Black leaf should be processed.
    if (to_remove->color == BLACK) {
      _rbTreeRemoveMaintain(tree, to_remove->parent, dir);
    }
  }
  // Case 2: node has child on both sides
  else if (to_remove->child[LEFT] && to_remove->child[RIGHT]) {
    binary_node *y = _findSuccessor(to_remove);
    enum color y_original_color = y->color;
    binary_node *x = NULL;

    if (y->parent != to_remove) {
      // y is not the direct right child of to_remove
      x = y->child[RIGHT];
      binary_tree_transplant(tree, y, y->child[RIGHT]);  // 将 y 与它的右子替换
      if (x) x->parent = y->parent;                      // 维护 x 的 parent
      y->child[RIGHT] = to_remove->child[RIGHT];
      if (y->child[RIGHT]) y->child[RIGHT]->parent = y;
    } else {
      // y is the direct right child of to_remove
      x = y->child[RIGHT];  // 可能为 NULL 或红色子节点
    }

    // Substitute to_remove to y
    binary_tree_transplant(tree, to_remove, y);
    y->child[LEFT] = to_remove->child[LEFT];
    if (y->child[LEFT]) y->child[LEFT]->parent = y;
    y->color = to_remove->color;

    // If the original color of y is black, then make a double black repair for
    // x.
    if (y_original_color == BLACK) {
      _rbTreeRemoveMaintain(tree, x ? x->parent : NULL,
                            x && x == x->parent->child[LEFT] ? LEFT : RIGHT);
    }
  }
  // Case 3: node has child on the left
  else if (to_remove->child[LEFT]) {
    assert(!to_remove->child[RIGHT]);
    binary_node *child = to_remove->child[LEFT];
    binary_tree_transplant(tree, to_remove, child);
    child->color = BLACK;
    to_remove->child[LEFT] = NULL;
  }
  // Case 4: node has child on the right
  else if (to_remove->child[RIGHT]) {
    assert(!to_remove->child[LEFT]);
    binary_node *child = to_remove->child[RIGHT];
    binary_tree_transplant(tree, to_remove, child);
    assert(child->color == RED);
    child->color = BLACK;
    to_remove->child[RIGHT] = NULL;
  }
  if (tree->root) {
    tree->root->color = BLACK;
  }
  to_remove->parent = NULL;
  to_remove->child[LEFT] = NULL;
  to_remove->child[RIGHT] = NULL;
  return to_remove;
}

#define MAX_OF(A, B) (((A) < (B)) ? (B) : (A))

typedef struct sequence_list {
  void *data;
  size_t size;
  size_t capacity;
} sequence_list;

size_t sequence_list_size(sequence_list *lst) { return lst->size; }

#define INIT_SEQUENCE_LIST(TYPE, LIST_PTR) \
  do {                                     \
    void *dat = malloc(sizeof(TYPE));      \
    if (dat == NULL) {                     \
      perror("Fail to init the list.");    \
    } else {                               \
      ((LIST_PTR)->capacity) = 1;          \
      ((LIST_PTR)->size) = 0;              \
      ((LIST_PTR)->data) = dat;            \
    }                                      \
  } while (0)

#define SEQUENCE_LIST_PUSH_BACK(TYPE, LIST_PTR, VAL)                  \
  do {                                                                \
    bool flag = true;                                                 \
    if (((LIST_PTR)->size) == ((LIST_PTR)->capacity)) {               \
      size_t new_capacity =                                           \
          MAX_OF(((LIST_PTR)->size) + 1, ((LIST_PTR)->capacity) * 2); \
      void *new_data =                                                \
          realloc(((LIST_PTR)->data), new_capacity * sizeof(TYPE));   \
      if (new_data == NULL) {                                         \
        perror("Fail to alloc extra memory, rollback...");            \
        flag = false;                                                 \
      } else {                                                        \
        ((LIST_PTR)->data) = new_data;                                \
        ((LIST_PTR)->capacity) = new_capacity;                        \
      }                                                               \
    }                                                                 \
    if (flag) {                                                       \
      ((TYPE *)((LIST_PTR)->data))[((LIST_PTR)->size)] = (TYPE)(VAL); \
      ((LIST_PTR)->size)++;                                           \
    }                                                                 \
  } while (0)

#define SEQUENCE_LIST_REFERENCE(TYPE, LIST_PTR, POSITION) \
  (((TYPE *)((LIST_PTR)->data))[(POSITION) % ((LIST_PTR)->size)])

#define SEQUENCE_LIST_AT(TYPE, LIST_PTR, POSITION) \
  (((TYPE *)((LIST_PTR)->data)) + (POSITION))

#define SEQUENCE_LIST_POP_BACK(TYPE, LIST_PTR)                      \
  do {                                                              \
    ((LIST_PTR)->size)--;                                           \
    if (((LIST_PTR)->capacity) > (((LIST_PTR)->size) * 4 + 1)) {    \
      size_t new_capacity = MAX_OF(((LIST_PTR)->size) * 4 + 1, 1);  \
      void *new_data =                                              \
          realloc(((LIST_PTR)->data), new_capacity * sizeof(TYPE)); \
      if (new_data == NULL) {                                       \
        perror("Fail to alloc new memory, continue...");            \
      } else {                                                      \
        ((LIST_PTR)->data) = new_data;                              \
        ((LIST_PTR)->capacity) = new_capacity;                      \
      }                                                             \
    }                                                               \
  } while (0)

#define SEQUENCE_LIST_INSERT(TYPE, LIST_PTR, POS, VAL)                        \
  do {                                                                        \
    bool flag = true;                                                         \
    size_t old_size = (LIST_PTR)->size;                                       \
    size_t new_size = ((LIST_PTR)->size) + 1;                                 \
    if (new_size > ((LIST_PTR)->capacity)) {                                  \
      size_t new_capacity = MAX_OF((((LIST_PTR)->capacity) * 2), (new_size)); \
      void *new_data =                                                        \
          realloc(((LIST_PTR)->data), new_capacity * sizeof(TYPE));           \
      if (new_data == NULL) {                                                 \
        perror("Fail to alloc new memory, rollback...");                      \
        flag = false;                                                         \
      } else {                                                                \
        ((LIST_PTR)->data) = new_data;                                        \
        ((LIST_PTR)->capacity) = new_capacity;                                \
      }                                                                       \
    }                                                                         \
    if (flag) {                                                               \
      ((LIST_PTR)->size) = new_size;                                          \
      TYPE *data = ((TYPE *)((LIST_PTR)->data));                              \
      memmove((void *)((TYPE *)(data) + ((POS) + 1)),                         \
              ((void *)((TYPE *)(data) + (POS))), ((old_size) - (POS)));      \
      data[(POS)] = (VAL);                                                    \
    }                                                                         \
  } while (0)

#define SEQUENCE_LIST_REMOVE(TYPE, LIST_PTR, POS)                   \
  do {                                                              \
    memmove(((void *)(((TYPE *)((LIST_PTR)->data)) + (POS))),       \
            ((void *)(((TYPE *)((LIST_PTR)->data) + (POS) + 1))),   \
            (((LIST_PTR)->size) - 1 - (POS)));                      \
    ((LIST_PTR)->size)--;                                           \
    if (((LIST_PTR)->capacity) > (((LIST_PTR)->size) * 4 + 1)) {    \
      size_t new_capacity = MAX_OF(((LIST_PTR)->size) * 4 + 1, 1);  \
      void *new_data =                                              \
          realloc(((LIST_PTR)->data), new_capacity * sizeof(TYPE)); \
      if (new_data == NULL) {                                       \
        perror("Fail to alloc new memory, continue...");            \
      } else {                                                      \
        ((LIST_PTR)->data) = new_data;                              \
        ((LIST_PTR)->capacity) = new_capacity;                      \
      }                                                             \
    }                                                               \
  } while (0)

#define SEQUENCE_LIST_RESERVE(TYPE, LIST_PTR, NEW_SIZE)                        \
  do {                                                                         \
    size_t new_capacity = (NEW_SIZE);                                          \
    void *new_data = realloc(((LIST_PTR)->data), new_capacity * sizeof(TYPE)); \
    if (new_data == NULL) {                                                    \
      perror("Fail to alloc new memory, rollback...");                         \
    } else {                                                                   \
      ((LIST_PTR)->data) = new_data;                                           \
      ((LIST_PTR)->capacity) = new_capacity;                                   \
    }                                                                          \
  } while (0)

#define SEQUENCE_LIST_RESIZE(TYPE, LIST_PTR, NEW_SIZE)              \
  do {                                                              \
    bool flag = true;                                               \
    if ((NEW_SIZE) >= ((LIST_PTR)->capacity)) {                     \
      size_t new_capacity =                                         \
          MAX_OF((NEW_SIZE) + 1, ((LIST_PTR)->capacity) * 2);       \
      void *new_data =                                              \
          realloc(((LIST_PTR)->data), new_capacity * sizeof(TYPE)); \
      if (new_data == NULL) {                                       \
        perror("Fail to alloc extra memory, rollback...");          \
        flag = false;                                               \
      } else {                                                      \
        ((LIST_PTR)->data) = new_data;                              \
        ((LIST_PTR)->capacity) = new_capacity;                      \
      }                                                             \
    }                                                               \
    if (flag) {                                                     \
      ((LIST_PTR)->size) = (NEW_SIZE);                              \
    }                                                               \
  } while (0)

#define SEQUENCE_LIST_SHRINK(TYPE, LIST_PTR)                                   \
  do {                                                                         \
    size_t new_capacity = MAX_OF(((LIST_PTR)->size), 1);                       \
    void *new_data = realloc(((LIST_PTR)->data), new_capacity * sizeof(TYPE)); \
    if (new_data == NULL) {                                                    \
      perror("Fail to alloc new memory, continue...");                         \
    } else {                                                                   \
      ((LIST_PTR)->data) = new_data;                                           \
      ((LIST_PTR)->capacity) = new_capacity;                                   \
    }                                                                          \
  } while (0)

void destroy_sequence_list(sequence_list *lst) {
  free(lst->data);
  lst->capacity = 0;
  lst->size = 0;
}

bool sequence_list_empty(sequence_list *lst) { return lst->size == 0; }

// Sequence stack

typedef sequence_list sequence_stack;

#define INIT_SEQUENCE_STACK(TYPE, STACK_PTR) INIT_SEQUENCE_LIST(TYPE, STACK_PTR)

#define SEQUENCE_STACK_PUSH(TYPE, STACK_PTR, VAL) \
  SEQUENCE_LIST_PUSH_BACK(TYPE, STACK_PTR, VAL)

#define SEQUENCE_STACK_TOP(TYPE, STACK_PTR) \
  SEQUENCE_LIST_REFERENCE(TYPE, STACK_PTR, (((STACK_PTR)->size) - 1))

#define SEQUENCE_STACK_POP(TYPE, STACK_PTR) \
  SEQUENCE_LIST_POP_BACK(TYPE, STACK_PTR)

void destroy_sequence_stack(sequence_stack *stack) {
  destroy_sequence_list(stack);
}

bool sequence_stack_empty(sequence_stack *stack) {
  return sequence_list_empty(stack);
}

// Algorithm

typedef struct encoder {
  char c;
  // node_in_huffman points to the `huff` member in huffman_node
  binary_node *node_in_huffman;
  char *code;
  binary_node node;
} encoder;

int encoder_cmp(const binary_node *node1, const binary_node *node2) {
  encoder *e1 = CONTAINER_OF(encoder, node, node1),
          *e2 = CONTAINER_OF(encoder, node, node2);
  if (e1->c < e2->c) {
    return -1;
  } else if (e1->c == e2->c) {
    return 0;
  } else {
    return 1;
  }
}

typedef struct huffman_node {
  size_t weight;
  binary_node huff;
  binary_node node;
} huffman_node;

int huff_cmp(const binary_node *n1, const binary_node *n2) {
  huffman_node *h1 = CONTAINER_OF(huffman_node, node, n1),
               *h2 = CONTAINER_OF(huffman_node, node, n2);
  if (h1->weight < h2->weight) {
    return -1;
  } else if (h1->weight == h2->weight) {
    return 0;
  } else {
    return 1;
  }
}

static inline void set_huff(binary_node *n1) {
  binary_node *h1 = &CONTAINER_OF(huffman_node, node, n1)->huff;
  h1->parent = NULL;
  h1->child[LEFT] = NULL;
  h1->child[RIGHT] = NULL;
}

void construct_huffman_tree(binary_tree *target, rb_tree *current_heap) {
  binary_tree_traverse(current_heap, set_huff, IN);
  while (current_heap->size > 1) {
    binary_node *node1 = bst_get_smallest(current_heap);
    rb_tree_remove(current_heap, node1, huff_cmp);
    binary_node *node2 = bst_get_smallest(current_heap);
    rb_tree_remove(current_heap, node2, huff_cmp);
    binary_node *new_node = MAKE_BINARY_NODE(huffman_node, node);
    binary_node *huff1 = &CONTAINER_OF(huffman_node, node, node1)->huff,
                *huff2 = &CONTAINER_OF(huffman_node, node, node2)->huff,
                *huff_p = &CONTAINER_OF(huffman_node, node, new_node)->huff;
    huff_p->child[LEFT] = huff1, huff_p->child[RIGHT] = huff2,
    huff1->parent = huff_p, huff2->parent = huff_p;
    CONTAINER_OF(huffman_node, huff, huff_p)->weight =
        CONTAINER_OF(huffman_node, huff, huff1)->weight +
        CONTAINER_OF(huffman_node, huff, huff2)->weight;
    rb_tree_insert(current_heap, new_node, huff_cmp);
  }
  target->root = &CONTAINER_OF(huffman_node, node, current_heap->root)->huff;
}

void gen_encoder(rb_tree *encoders, binary_tree *huffman_tree) {
  void set_encoder(binary_node * cnode) {
    sequence_stack st;
    INIT_SEQUENCE_STACK(char, &st);
    size_t cnt = 0;
    encoder *e = CONTAINER_OF(encoder, node, cnode);
    binary_node *hnode = e->node_in_huffman;
    while (hnode != huffman_tree->root) {
      cnt++;
      binary_node *parent = hnode->parent;
      if (hnode == parent->child[LEFT]) {
        SEQUENCE_STACK_PUSH(char, &st, '0');
      } else if (hnode == parent->child[RIGHT]) {
        SEQUENCE_STACK_PUSH(char, &st, '1');
      }
      hnode = parent;
    }
    char *res = malloc(cnt + 1);
    size_t i = 0;
    while (!sequence_stack_empty(&st)) {
      res[i] = SEQUENCE_STACK_TOP(char, &st);
      SEQUENCE_STACK_POP(char, &st);
      i++;
    }
    res[i] = 0;
    e->code = res;
    destroy_sequence_stack(&st);
  }
  binary_tree_traverse(encoders, set_encoder, IN);
}

char *get_code(rb_tree *encoders, char c) {
  encoder coder;
  coder.c = c;
  binary_node *n = rb_tree_find_node(encoders, &coder.node, encoder_cmp);
  return CONTAINER_OF(encoder, node, n)->code;
}

static inline void free_encoder(binary_node *n) {
  if (n) {
    assert(CONTAINER_OF(encoder, node, n)->code);
    free(CONTAINER_OF(encoder, node, n)->code);
    CONTAINER_OF(encoder, node, n)->code = NULL;
  }
}

char *release_resources(binary_tree *encoders, binary_tree *huff) {
  DESTROY_BINARY_TREE(huffman_node, huff, huff);
  binary_tree_traverse(encoders, free_encoder, IN);
  DESTROY_BINARY_TREE(encoder, node, encoders);
}

int main(void) {
  size_t num_char;
  scanf("%lu", &num_char);
  rb_tree char_tree, weight_heap;
  init_rb_tree(&char_tree);
  init_rb_tree(&weight_heap);
  binary_node *encoders[num_char];
  for (size_t i = 0; i < num_char; i++) {
    binary_node *to_ins = MAKE_BINARY_NODE(encoder, node);
    char c;
    do {
      scanf("%c", &c);
    } while (isspace(c));
    CONTAINER_OF(encoder, node, to_ins)->c = c;
    rb_tree_insert(&char_tree, to_ins, encoder_cmp);
    encoders[i] = to_ins;
  }
  for (size_t i = 0; i < num_char; i++) {
    binary_node *to_proc = encoders[i];
    CONTAINER_OF(encoder, node, to_proc)->node_in_huffman =
        MAKE_BINARY_NODE(huffman_node, huff);
    binary_node *node_in_heap =
        &CONTAINER_OF(huffman_node, huff,
                      CONTAINER_OF(encoder, node, to_proc)->node_in_huffman)
             ->node;
    size_t weight;
    scanf("%lu", &weight);
    CONTAINER_OF(huffman_node, node, node_in_heap)->weight = weight;
    rb_tree_insert(&weight_heap, node_in_heap, huff_cmp);
  }
  binary_tree huffman;
  init_binary_tree(&huffman);
  construct_huffman_tree(&huffman, &weight_heap);
  gen_encoder(&char_tree, &huffman);
  char str[101];
  scanf("%s", str);
  size_t len = strlen(str);
  for (size_t i = 0; i < len; i++) {
    char c = str[i];
    char *code = get_code(&char_tree, c);
    printf("%s", code);
  }
  putchar('\n');
  printf("%s\n", str);
  release_resources(&char_tree, &huffman);
}
