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

#define FREE_BINARY_NODE(NODE_TYPE, MEMBER, NODE_PTR) \
  (free(CONTAINER_OF(NODE_TYPE, MEMBER, NODE_PTR)))

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

#ifdef ENABLE_QUEUE

#include "./linked_list.c"

void binary_node_level_order(binary_node *root, void (*func)(binary_node *)) {
  typedef struct {
    binary_node *target;
    intrusive_node node;
  } bfs_node;
  linked_queue queue;
  init_linked_queue(&queue);
  intrusive_node *nRoot = MAKE_NODE(bfs_node, node);
  CONTAINER_OF(bfs_node, node, nRoot)->target = root;
  linked_queue_push(&queue, nRoot);
  nRoot = NULL;
  while (!linked_queue_empty(&queue)) {
    intrusive_node *node_to_proc = linked_queue_pop(&queue);

    func(CONTAINER_OF(bfs_node, node, node_to_proc)->target);
    if (CONTAINER_OF(bfs_node, node, node_to_proc)->target->child[LEFT] !=
        NULL) {
      intrusive_node *nNode = MAKE_NODE(bfs_node, node);
      CONTAINER_OF(bfs_node, node, nNode)->target =
          CONTAINER_OF(bfs_node, node, node_to_proc)->target->child[LEFT];
      linked_queue_push(&queue, nNode);
    }
    if (CONTAINER_OF(bfs_node, node, node_to_proc)->target->child[RIGHT] !=
        NULL) {
      intrusive_node *nNode = MAKE_NODE(bfs_node, node);
      CONTAINER_OF(bfs_node, node, nNode)->target =
          CONTAINER_OF(bfs_node, node, node_to_proc)->target->child[RIGHT];
      linked_queue_push(&queue, nNode);
    }
    RELEASE_NODE(bfs_node, node, node_to_proc);
  }
}

#endif

binary_node *construct_binary_tree(binary_node **node_list_mid,
                                   binary_node **node_list, size_t len,
                                   enum traverse_order seq) {
  switch (seq) {
    case PRE: {
      if (len < 1) {
        return NULL;
      }
      binary_node *root = node_list[0];
      if (len == 1) {
        root->child[LEFT] = NULL;
        root->child[RIGHT] = NULL;
        return root;
      }
      size_t pos;
      for (pos = 0; pos < len; pos++) {
        if (node_list_mid[pos] == root) {
          break;
        }
      }
      root->child[LEFT] =
          construct_binary_tree(node_list_mid, node_list + 1, pos, seq);
      root->child[RIGHT] = construct_binary_tree(
          node_list_mid + pos + 1, node_list + pos + 1, len - pos - 1, seq);
      return root;
    }
    case POST: {
      if (len < 1) {
        return NULL;
      }
      binary_node *root = node_list[len - 1];
      if (len == 1) {
        root->child[LEFT] = NULL;
        root->child[RIGHT] = NULL;
        return root;
      }
      size_t pos;
      for (pos = 0; pos < len; pos++) {
        if (node_list_mid[pos] == root) {
          break;
        }
      }
      root->child[LEFT] =
          construct_binary_tree(node_list_mid, node_list, pos, seq);
      root->child[RIGHT] = construct_binary_tree(
          node_list_mid + pos + 1, node_list + pos, len - pos - 1, seq);
      return root;
    }
    case IN: {
      assert(0);
    }
    default: {
      assert(0);
    }
  }
}

struct _internal_node_pair {
  binary_node *first, *second;
};

static inline struct _internal_node_pair _bst_small_big(
    binary_node *root, int (*cmp)(const binary_node *, const binary_node *)) {
  struct _internal_node_pair res = {NULL, NULL};
  if (root == NULL) {
    return (struct _internal_node_pair){.first = NULL, .second = NULL};
  }

  if (root->child[LEFT]) {
    struct _internal_node_pair left_pair =
        _bst_small_big(root->child[LEFT], cmp);
    if (left_pair.first == NULL || left_pair.second == NULL) {
      return (struct _internal_node_pair){.first = NULL, .second = NULL};
    } else if (cmp(left_pair.second, root) >= 0) {
      return (struct _internal_node_pair){NULL, NULL};
    } else {
      res.first = left_pair.first;
    }
  } else {
    res.first = root;
  }

  if (root->child[RIGHT]) {
    struct _internal_node_pair right_pair =
        _bst_small_big(root->child[RIGHT], cmp);
    if (right_pair.first == NULL || right_pair.second == NULL) {
      return (struct _internal_node_pair){NULL, NULL};
    } else if (cmp(root, right_pair.first) >= 0) {
      return (struct _internal_node_pair){NULL, NULL};
    } else {
      res.second = right_pair.second;
    }
  } else {
    res.second = root;
  }

  return res;
}

bool binary_node_is_bst_root(binary_node *root,
                             int (*cmp)(const binary_node *,
                                        const binary_node *)) {
  if (root == NULL) {
    return true;
  }

  struct _internal_node_pair node = _bst_small_big(root, cmp);
  if (node.first == NULL || node.second == NULL) {
    return false;
  }
  return true;
}

bool binary_tree_is_bst(binary_tree *tree,
                        int (*cmp)(const binary_node *, const binary_node *)) {
  return binary_node_is_bst_root(tree->root, cmp);
}

// Algorithm

// Construct a struct of the node.
typedef struct {
  char dat;
  binary_node node;
} cnode_t;

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

// Sequence list

#ifndef SEQUENCE_LIST_H__
#define SEQUENCE_LIST_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// Sqeuence queue

typedef struct sequence_queue {
  size_t begin_pos;
  size_t end_pos;
  sequence_list lst;
} sequence_queue;

#define INIT_SEQUENCE_QUEUE(TYPE, Q_PTR, CAPACITY)           \
  do {                                                       \
    INIT_SEQUENCE_LIST(TYPE, (&((Q_PTR)->lst)));             \
    SEQUENCE_LIST_RESIZE(TYPE, (&((Q_PTR)->lst)), CAPACITY); \
    ((Q_PTR)->begin_pos) = 0;                                \
    ((Q_PTR)->end_pos) = 0;                                  \
  } while (0)

bool sequence_queue_empty(sequence_queue *queue) {
  return queue->begin_pos == queue->end_pos;
}

bool sequence_queue_full(sequence_queue *queue) {
  return (queue->end_pos - queue->begin_pos) >= queue->lst.size;
}

#define SEQUENCE_QUEUE_PUSH(TYPE, Q_PTR, VAL)                                  \
  do {                                                                         \
    if (!sequence_queue_full((Q_PTR))) {                                       \
      SEQUENCE_LIST_REFERENCE(TYPE, (&((Q_PTR)->lst)), (((Q_PTR)->end_pos))) = \
          (VAL);                                                               \
      ((Q_PTR)->end_pos)++;                                                    \
    } else {                                                                   \
      perror("The queue is full, giving up...");                               \
    }                                                                          \
  } while (0)

void sequence_queue_pop(sequence_queue *queue) {
  if (!sequence_queue_empty(queue)) {
    (queue->begin_pos)++;
    if (queue->begin_pos >= queue->lst.size) {
      queue->begin_pos -= queue->lst.size;
      queue->end_pos -= queue->lst.size;
    }
  } else {
    perror("The queue is already empty, giving up...");
  }
}
#define SEQUENCE_QUEUE_FRONT(TYPE, Q_PTR) \
  SEQUENCE_LIST_REFERENCE(TYPE, (&((Q_PTR)->lst)), ((Q_PTR)->begin_pos))

void destroy_sequence_queue(sequence_queue *queue) {
  destroy_sequence_list(&queue->lst);
  queue->begin_pos = 0;
  queue->end_pos = 0;
}

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

#endif

// Module

static inline bool isNotNull(binary_node *node) {
  return node ? true : false;
}

bool process(binary_tree *tree) {
  if (tree->root == NULL) {
    return true;
  }
  if (!tree->root->child[LEFT] && !tree->root->child[RIGHT]) {
    return true;
  }
  if (!tree->root->child[LEFT] || !tree->root->child[RIGHT]) {
    return false;
  }

  __auto_type lRoot = tree->root->child[LEFT];
  __auto_type rRoot = tree->root->child[RIGHT];
  typedef binary_node *bNodePtr;
  sequence_stack lStk, rStk;
  INIT_SEQUENCE_STACK(bNodePtr, &lStk);
  INIT_SEQUENCE_STACK(bNodePtr, &rStk);
  bool res = true;

  // To be realized.
  SEQUENCE_STACK_PUSH(bNodePtr, &lStk, lRoot);
  SEQUENCE_STACK_PUSH(bNodePtr, &rStk, rRoot);
  while (!sequence_stack_empty(&lStk)) {
    if (sequence_stack_empty(&rStk)) {
      res = false;
      goto release;
    }
    bNodePtr lNode = SEQUENCE_STACK_TOP(bNodePtr, &lStk);
    SEQUENCE_STACK_POP(bNodePtr, &lStk);
    bNodePtr rNode = SEQUENCE_STACK_TOP(bNodePtr, &rStk);
    SEQUENCE_STACK_POP(bNodePtr, &rStk);
    if (isNotNull(lNode) ^ isNotNull(rNode)) {
      res = false;
      goto release;
    }
    if (isNotNull(lNode)) {
      SEQUENCE_STACK_PUSH(bNodePtr, &lStk, lNode->child[LEFT]);
      SEQUENCE_STACK_PUSH(bNodePtr, &lStk, lNode->child[RIGHT]);
      SEQUENCE_STACK_PUSH(bNodePtr, &rStk, rNode->child[RIGHT]);
      SEQUENCE_STACK_PUSH(bNodePtr, &rStk, rNode->child[LEFT]);
    }
  }

  if (!sequence_stack_empty(&rStk)) {
    res = false;
    goto release;
  }

release:
  destroy_sequence_stack(&lStk);
  destroy_sequence_stack(&rStk);
  return res;
}

int main(void) {
  binary_tree tree;
  construct_btree_local(&tree);
  __auto_type r = process(&tree);
  printf("%s\n", r ? "Is symmetric" : "Is not symmetric");
}
