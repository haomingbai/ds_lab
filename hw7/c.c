#include "./framework.c"

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

static inline bool isNotNull(binary_node *node) { return node ? true : false; }

// 只判断树形是否对称
bool process(binary_tree *tree) {
  // 空树对称
  if (tree->root == NULL) {
    return true;
  }
  // 单节点树对称
  if (!tree->root->child[LEFT] && !tree->root->child[RIGHT]) {
    return true;
  }
  // 单分支树不对称
  if (!tree->root->child[LEFT] || !tree->root->child[RIGHT]) {
    return false;
  }

  // 通过栈模拟递归, 如果入栈时间节点相同, 则树对称
  // 左右两个子树遍历方向相反
  __auto_type lRoot = tree->root->child[LEFT];
  __auto_type rRoot = tree->root->child[RIGHT];
  typedef binary_node *bNodePtr;
  sequence_stack lStk, rStk;
  INIT_SEQUENCE_STACK(bNodePtr, &lStk);
  INIT_SEQUENCE_STACK(bNodePtr, &rStk);
  bool res = true;

  SEQUENCE_STACK_PUSH(bNodePtr, &lStk, lRoot);
  SEQUENCE_STACK_PUSH(bNodePtr, &rStk, rRoot);

  // 遍历
  while (!sequence_stack_empty(&lStk)) {
    // 如果有一边遍历提前结束, 则树不对称
    if (sequence_stack_empty(&rStk)) {
      res = false;
      goto release;
    }

    // 获取栈顶节点
    bNodePtr lNode = SEQUENCE_STACK_TOP(bNodePtr, &lStk);
    SEQUENCE_STACK_POP(bNodePtr, &lStk);
    bNodePtr rNode = SEQUENCE_STACK_TOP(bNodePtr, &rStk);
    SEQUENCE_STACK_POP(bNodePtr, &rStk);

    // 若有一个节点为空, 树不对称
    if (isNotNull(lNode) ^ isNotNull(rNode)) {
      res = false;
      goto release;
    }

    // 如果左右都不为空, 按照相反顺序入栈
    if (isNotNull(lNode)) {
      SEQUENCE_STACK_PUSH(bNodePtr, &lStk, lNode->child[LEFT]);
      SEQUENCE_STACK_PUSH(bNodePtr, &lStk, lNode->child[RIGHT]);
      SEQUENCE_STACK_PUSH(bNodePtr, &rStk, rNode->child[RIGHT]);
      SEQUENCE_STACK_PUSH(bNodePtr, &rStk, rNode->child[LEFT]);
    }
  }

  // 如果有一边遍历提前结束, 则树不对称
  if (!sequence_stack_empty(&rStk)) {
    res = false;
    goto release;
  }

release:  // 资源释放
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
