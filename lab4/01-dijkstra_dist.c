#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// GNU C 扩展 Likely 分支预测优化支持
#ifdef __GNUC__
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif

// 容器支持, 帮助实现侵入式数据结构
#define CONTAINER_OF(NODE_TYPE, MEMBER, NODE_PTR) \
  ((NODE_TYPE *)((char *)(NODE_PTR) - (offsetof(NODE_TYPE, MEMBER))))

// 结构体定义
typedef struct binary_node binary_node;
typedef binary_node binary_node;

// 树旋转方向和颜色支持
enum direction { LEFT = 0, RIGHT = 1 };
enum color { RED = 0, BLACK = 1 };

// 红黑树和二叉树节点
struct binary_node {
  binary_node *parent;
  binary_node *child[2];
  enum color color;
};

// 二叉树的遍历顺序.
enum traverse_order { PRE, IN, POST };

// 二叉树类定义
typedef struct binary_tree {
  binary_node *root;
  size_t size;
} binary_tree;

// 内部别名定义
typedef binary_tree bTree;

// 构造函数定义
int init_binary_tree(binary_tree *tree) {
  tree->size = 0, tree->root = NULL;
  return 0;
}

// 二叉树生成函数定义, 方便在堆上直接构建红黑树
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

// 二叉树的插入, 注意不会改变二叉树存储的大小, 大小信息请手动更新
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

// 工具函数: 二叉树的移植. 移植功能在二叉树的构造中及其重要.
// 一个最小化功能的移植应当是单项的. 即在移植过程中,
// 新节点会被放置在旧节点的位置, 即所在位置的父节点的儿子需要是新节点,
// 但是新节点的父亲并没有被设置成旧节点.
binary_node *binary_tree_transplant(binary_tree *tree, binary_node *old_node,
                                    binary_node *new_node) {
  binary_node *parent = old_node->parent;
  if (new_node != NULL) {
    new_node->parent = parent;
  }
  if (parent == NULL) {
    // 移植的位置在树根
    assert(tree->root == old_node);
    tree->root = new_node;
  } else {
    // 移植在树枝或者树干
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

// 带有偏移量的内存分配函数, 使用malloc实现,
// 但是同时考虑到侵入式数据结构经常需要返回偏移地址的特性. 这个函数不对外暴露,
// 仅供内部使用.
static inline void *__alloc_with_offset(size_t size, size_t offset) {
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

// 创造新节点
#define MAKE_BINARY_NODE(NODE_TYPE, MEMBER)              \
  ((binary_node *)__alloc_with_offset(sizeof(NODE_TYPE), \
                                      offsetof(NODE_TYPE, MEMBER)))

// 释放旧节点
#define FREE_BINARY_NODE(NODE_TYPE, MEMBER, NODE_PTR) \
  (free(CONTAINER_OF(NODE_TYPE, MEMBER, NODE_PTR)))

// 交换左右孩子
void binary_node_switch_order(binary_node *node) {
  binary_node *tmp = node->child[LEFT];
  node->child[LEFT] = node->child[RIGHT];
  node->child[RIGHT] = tmp;
}

// 定义"最大"
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

// 确定二叉树树高
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

// 直接获取二叉树高度, 请在二叉树高度有人维护的前提下使用.
size_t binary_tree_get_size(binary_tree *tree) { return tree->size; }

size_t binary_node_get_size(binary_node *root) {
  if (root == NULL) {
    return 0;
  }
  return 1 + binary_node_get_size(root->child[LEFT]) +
         binary_node_get_size(root->child[RIGHT]);
}

// 二叉树获取树大小
size_t binary_tree_update_size(binary_tree *tree) {
  size_t res = binary_node_get_size(tree->root);
  tree->size = res;
  return res;
}

size_t binary_tree_gen_size(binary_tree *tree) {
  size_t res = binary_node_get_size(tree->root);
  return res;
}

// 内存释放函数...
void __release_with_offset(void *ptr, size_t offset) {
  if (ptr == NULL) {
    return;
  }
  free((void *)((char *)ptr - offset));
}

// 内存释放函数
void __free_binary_root_with_offset(binary_node *root, size_t offset) {
  if (root == NULL) {
    return;
  }
  __free_binary_root_with_offset(root->child[LEFT], offset);
  __free_binary_root_with_offset(root->child[RIGHT], offset);
  __release_with_offset(root, offset);
}

// 销毁侵入式二叉树
#define DESTROY_BINARY_TREE(NODE_TYPE, MEMBER, TREE_PTR) \
  __free_binary_root_with_offset(((TREE_PTR)->root),     \
                                 (offsetof(NODE_TYPE, MEMBER)))

// 工具函数: 二叉树的旋转
int binary_tree_rotate(binary_tree *tree, binary_node *node,
                       enum direction dir) {
  if (node == NULL || tree == NULL) {
    return -1;
  }
  // 旋转节点是树根
  if (node->parent == NULL) {
    assert(node == tree->root);

    switch (dir) {
      // 左旋代码
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
        // 悬挂新节点
        old_root->parent = new_root;
        new_root->parent = NULL;

        return 0;
      }
      // 右旋代码
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
        // 悬挂新节点
        old_root->parent = new_root;
        new_root->parent = NULL;

        return 0;
      }
      default: {
        return -3;
      }
    }
  }
  // 旋转节点不是树根
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

// 反转二叉树功能
void binary_tree_traverse(binary_tree *tree, void (*func)(binary_node *),
                          enum traverse_order order) {
  if (tree->root == NULL) {
    return;
  } else {
    binary_node_traverse(tree->root, func, order);
  }
}

// 二叉搜索树, 类声明和定义
typedef binary_tree bst;
typedef binary_node bst_node;

// 二叉搜索树的CURD和构造函数
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

// 在二叉搜索树中找到某个节点, 如果不存在返回`NULL`
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

// 获得树中的最小节点.
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

// 获得二叉搜索树中的最大节点
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

// Sequence list

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

// Linked list

#define CONTAINER_OF(NODE_TYPE, MEMBER, NODE_PTR) \
  ((NODE_TYPE *)((char *)(NODE_PTR) - (offsetof(NODE_TYPE, MEMBER))))

#define MAKE_NODE(NODE_TYPE, MEMBER) \
  ((intrusive_node *)(&(((NODE_TYPE *)(malloc(sizeof(NODE_TYPE))))->MEMBER)))

#define REMOVE_NODE_FROM_LIST(LIST_PTR, NODE_PTR) \
  (((LIST_PTR)->size--), _remove_Node_without_changing_size(NODE_PTR));

#define RELEASE_NODE(NODE_TYPE, MEMBER, NODE_PTR) \
  free(CONTAINER_OF(NODE_TYPE, MEMBER, NODE_PTR))

#define REMOVE_AND_RELEASE(NODE_TYPE, MEMBER, LIST_PTR, NODE_PTR) \
  do {                                                            \
    intrusive_node *_NODE_TO_DEL = NODE_PTR;                      \
    assert(_NODE_TO_DEL != (LIST_PTR)->head);                     \
    REMOVE_NODE_FROM_LIST(LIST_PTR, _NODE_TO_DEL);                \
    free(CONTAINER_OF(NODE_TYPE, MEMBER, _NODE_TO_DEL));          \
  } while (0)

#define IS_EMPTY(LIST_PTR) (!((LIST_PTR)->size))

#define DESTROY_LIST(NODE_TYPE, MEMBER, LIST_TYPE, LIST_PTR)      \
  do {                                                            \
    while ((LIST_PTR)->size) {                                    \
      assert(((LIST_PTR)->head)->next->prev == (LIST_PTR)->head); \
      REMOVE_AND_RELEASE(NODE_TYPE, MEMBER, LIST_PTR,             \
                         (((LIST_PTR)->head)->next));             \
    }                                                             \
    _free_empty_##LIST_TYPE##_(LIST_PTR);                         \
  } while (0)

#define INSERT_BEHIND(LIST_PTR, NODE_PTR, NEW_NODE_PTR) \
  do {                                                  \
    ((NEW_NODE_PTR)->prev) = (NODE_PTR);                \
    ((NEW_NODE_PTR)->next) = ((NODE_PTR)->next);        \
    (((NODE_PTR)->next)->prev) = (NEW_NODE_PTR);        \
    ((NODE_PTR)->next) = (NEW_NODE_PTR);                \
    ((LIST_PTR)->size)++;                               \
  } while (0)

#define INSERT_IN_FRONT_OF(LIST_PTR, NODE_PTR, NEW_NODE_PTR) \
  do {                                                       \
    ((NEW_NODE_PTR)->prev) = ((NODE_PTR)->prev);             \
    ((NEW_NODE_PTR)->next) = (NODE_PTR);                     \
    (((NODE_PTR)->prev)->next) = (NEW_NODE_PTR);             \
    ((NODE_PTR)->prev) = (NEW_NODE_PTR);                     \
    ((LIST_PTR)->size)++;                                    \
  } while (0)

#define INIT_LINKED_LIST(LIST_TYPE, LIST_PTR) (_init_##LIST_TYPE##_(LIST_PTR))

typedef struct intrusive_node {
  struct intrusive_node *prev;
  struct intrusive_node *next;

} intrusive_node;

typedef struct doubly_linked_list {
  intrusive_node *head;
  intrusive_node *tail;
  size_t size;
} doubly_linked_list;

typedef struct circular_linked_list {
  intrusive_node *head;
  size_t size;
} circular_linked_list;

void _init_doubly_linked_list_(doubly_linked_list *list_ptr);

void _init_circular_linked_list_(circular_linked_list *list_ptr);

void _free_empty_doubly_linked_list_(doubly_linked_list *list_ptr);

void _free_empty_circular_linked_list_(circular_linked_list *list_ptr);

void _init_doubly_linked_list_(doubly_linked_list *list_ptr) {
  intrusive_node *head_node = NULL, *tail_node = NULL;
  head_node = (intrusive_node *)malloc(sizeof(intrusive_node));
  tail_node = (intrusive_node *)malloc(sizeof(intrusive_node));

  if (head_node == 0 || tail_node == 0) {
    perror("Init doubly linked list failed!");
    free(tail_node);
    free(head_node);
    return;
  }

  head_node->next = tail_node;
  head_node->prev = NULL;
  tail_node->next = NULL;
  tail_node->prev = head_node;

  list_ptr->size = 0;
  list_ptr->head = head_node;
  list_ptr->tail = tail_node;
}

void _init_circular_linked_list_(circular_linked_list *list_ptr) {
  intrusive_node *head_node = 0;
  head_node = (intrusive_node *)malloc(sizeof(intrusive_node));

  if (head_node == 0) {
    perror("Init circular linked list failed!");
    free(head_node);
    return;
  }

  head_node->prev = head_node;
  head_node->next = head_node;

  list_ptr->head = head_node;
  list_ptr->size = 0;
}

void _free_empty_doubly_linked_list_(doubly_linked_list *list_ptr) {
  assert(list_ptr->size == 0);
  free(list_ptr->tail);
  free(list_ptr->head);
  list_ptr->tail = NULL;
  list_ptr->head = NULL;
}

intrusive_node *_remove_Node_without_changing_size(intrusive_node *node_ptr) {
  node_ptr->prev->next = node_ptr->next;
  node_ptr->next->prev = node_ptr->prev;
  node_ptr->prev = 0;
  node_ptr->next = 0;
  return node_ptr;
}

void _free_empty_circular_linked_list_(circular_linked_list *list_ptr) {
  assert(list_ptr->size == 0);
  free(list_ptr->head);
  list_ptr->head = NULL;
}

// Graph

typedef unsigned long DataType;

typedef struct to_edge {
  size_t destination;
  DataType length;
} to_edge;

typedef struct adjacent_list {
  sequence_list lst;
  size_t size;
  DataType inf;
} adjacent_list;

int init_adjacent_list(adjacent_list *obj, size_t node_num) {
  INIT_SEQUENCE_LIST(doubly_linked_list, &obj->lst);
  SEQUENCE_LIST_RESIZE(doubly_linked_list, &obj->lst, node_num);
  ;
  for (size_t i = 0; i < node_num; i++) {
    doubly_linked_list *it = SEQUENCE_LIST_AT(doubly_linked_list, &obj->lst, i);
    INIT_LINKED_LIST(doubly_linked_list, it);
  }
  obj->size = node_num;
  obj->inf = 1e9;
  return 0;
}

int init_adjacent_list_with_inf(adjacent_list *obj, size_t node_num,
                                DataType inf) {
  INIT_SEQUENCE_LIST(doubly_linked_list, &obj->lst);
  SEQUENCE_LIST_RESIZE(doubly_linked_list, &obj->lst, node_num);
  ;
  for (size_t i = 0; i < node_num; i++) {
    doubly_linked_list *it = SEQUENCE_LIST_AT(doubly_linked_list, &obj->lst, i);
    INIT_LINKED_LIST(doubly_linked_list, it);
  }
  obj->inf = inf;
  obj->size = node_num;
  return 0;
}

typedef struct edge_node {
  to_edge edge;
  intrusive_node node;
} edge_node;

int adjacent_list_add_edge(adjacent_list *obj, size_t from, size_t to,
                           DataType len) {
  if (from == to) {
    return -1;
  }
  if (len >= obj->inf) {
    return -2;
  }
  intrusive_node *node = MAKE_NODE(edge_node, node);
  CONTAINER_OF(edge_node, node, node)->edge.destination = to;
  CONTAINER_OF(edge_node, node, node)->edge.length = len;
  INSERT_IN_FRONT_OF(
      SEQUENCE_LIST_AT(doubly_linked_list, &obj->lst, from),
      SEQUENCE_LIST_AT(doubly_linked_list, &obj->lst, from)->tail, node);
  return 0;
}

int adjacent_list_remove_edge(adjacent_list *obj, size_t from, size_t to) {
  bool flag = false;
  for (intrusive_node *it =
           SEQUENCE_LIST_AT(doubly_linked_list, &obj->lst, from)->head;
       it != SEQUENCE_LIST_AT(doubly_linked_list, &obj->lst, from)->tail;) {
    if (CONTAINER_OF(edge_node, node, it)->edge.destination == to) {
      intrusive_node *old_it = it;
      it = it->next;
      REMOVE_AND_RELEASE(edge_node, node,
                         SEQUENCE_LIST_AT(doubly_linked_list, &obj->lst, from),
                         it);
      flag = true;
    } else {
      it = it->next;
    }
  }
  if (!flag) {
    return 0;
  } else {
    return -1;
  }
}

int destroy_adjacent_list(adjacent_list *obj) {
  for (size_t i = 0; i < obj->size; i++) {
    DESTROY_LIST(edge_node, node, doubly_linked_list,
                 SEQUENCE_LIST_AT(doubly_linked_list, &obj->lst, i));
  }
  destroy_sequence_list(&obj->lst);
  return 0;
}

// Dijkstra
typedef struct distance_node {
  DataType distance;
  size_t pos;
  binary_node node;
} distance_node;

int distance_node_cmp(const binary_node *a, const binary_node *b) {
  if (CONTAINER_OF(distance_node, node, a)->distance >
      CONTAINER_OF(distance_node, node, b)->distance) {
    return -1;
  } else if (CONTAINER_OF(distance_node, node, a)->distance <
             CONTAINER_OF(distance_node, node, b)->distance) {
    return 1;
  } else {
    if (CONTAINER_OF(distance_node, node, a)->pos > CONTAINER_OF(distance_node, node, b)->pos) {
      return -1;
    } else if (CONTAINER_OF(distance_node, node, a)->pos < CONTAINER_OF(distance_node, node, b)->pos) {
      return 1;
    } else {
      return 0;
    }
  }
}

int dijkstra(adjacent_list *graph, sequence_list *distance,
             sequence_list *prevs, size_t from) {
  if (graph->size == 0) {
    return -1;
  }

  // Initialize required resources
  SEQUENCE_LIST_RESIZE(DataType, distance, graph->size);
  for (size_t i = 0; i < graph->size; i++) {
    SEQUENCE_LIST_REFERENCE(DataType, distance, i) = graph->inf;
  }
  SEQUENCE_LIST_RESIZE(size_t, prevs, graph->size);
  for (size_t i = 0; i < graph->size; i++) {
    SEQUENCE_LIST_REFERENCE(size_t, prevs, i) = SIZE_MAX;
  }
  SEQUENCE_LIST_REFERENCE(DataType, distance, from) = 0;
  rb_tree pqueue;  // Priority queue
  init_rb_tree(&pqueue);
  binary_node *node = MAKE_BINARY_NODE(distance_node, node);
  CONTAINER_OF(distance_node, node, node)->distance = 0;
  CONTAINER_OF(distance_node, node, node)->pos = from;
  rb_tree_insert(&pqueue, node, distance_node_cmp);
  node = NULL;
  bool *visited = calloc(graph->size, sizeof(bool));
  size_t cnt = 0;
  bool success = true;

  while (binary_tree_get_size(&pqueue) != 0) {
    binary_node *it = bst_get_greatest(&pqueue);
    size_t to_proc = CONTAINER_OF(distance_node, node, it)->pos;
    DataType dist = CONTAINER_OF(distance_node, node, it)->distance;
    rb_tree_remove(&pqueue, it, distance_node_cmp);
    FREE_BINARY_NODE(distance_node, node, it);
    it = NULL;
    if (visited[to_proc]) {
      continue;
    }
    if (cnt >= graph->size) {
      break;
    }
    if (dist >= graph->inf) {
      success = false;
      break;
    }
    cnt++;
    visited[to_proc] = true;

    for (intrusive_node *it =
             SEQUENCE_LIST_AT(doubly_linked_list, &graph->lst, to_proc)
                 ->head->next;
         it != SEQUENCE_LIST_AT(doubly_linked_list, &graph->lst, to_proc)->tail;
         it = it->next) {
      DataType edge_len = CONTAINER_OF(edge_node, node, it)->edge.length;
      size_t to_update = CONTAINER_OF(edge_node, node, it)->edge.destination;
      if (!visited[to_update] && edge_len < graph->inf &&
          edge_len < graph->inf -
                         SEQUENCE_LIST_REFERENCE(DataType, distance, to_proc) &&
          SEQUENCE_LIST_REFERENCE(DataType, distance, to_proc) + edge_len <
              SEQUENCE_LIST_REFERENCE(DataType, distance, to_update)) {
        binary_node *node_to_insert = MAKE_BINARY_NODE(distance_node, node);
        DataType final_dist =
            SEQUENCE_LIST_REFERENCE(DataType, distance, to_proc) + edge_len;
        CONTAINER_OF(distance_node, node, node_to_insert)->distance =
            final_dist;
        CONTAINER_OF(distance_node, node, node_to_insert)->pos = to_update;
        int insert_res = rb_tree_insert(&pqueue, node_to_insert, distance_node_cmp);
        if (insert_res) {
          FREE_BINARY_NODE(distance_node, node, node_to_insert);
        }
        node_to_insert = NULL;
        SEQUENCE_LIST_REFERENCE(DataType, distance, to_update) = final_dist;
        SEQUENCE_LIST_REFERENCE(size_t, prevs, to_update) = to_proc;
      }
    }
  }

  free(visited);
  DESTROY_BINARY_TREE(distance_node, node, &pqueue);
  if (success) {
    return 0;
  } else {
    return -1;
  }
}

int solve() {
  adjacent_list graph;
  size_t node_num;
  scanf("%lu", &node_num);
  init_adjacent_list_with_inf(&graph, node_num, 10000);
  for (size_t i = 0; i < node_num; i++) {
    for (size_t j = 0; j < node_num; j++) {
      DataType length;
      scanf("%lu", &length);
      adjacent_list_add_edge(&graph, i, j, length);
    }
  }
  size_t from = 0;

  sequence_list distance, prevs;
  INIT_SEQUENCE_LIST(DataType, &distance);
  INIT_SEQUENCE_LIST(size_t, &prevs);
  dijkstra(&graph, &distance, &prevs, from);

  for (size_t i = 0; i < node_num; i++) {
    printf("%lu\n", SEQUENCE_LIST_REFERENCE(DataType, &distance, i));
  }

  destroy_sequence_list(&prevs);
  destroy_sequence_list(&distance);
  destroy_adjacent_list(&graph);
  return 0;
}

int main() { solve(); }
