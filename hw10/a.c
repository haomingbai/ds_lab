#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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


void list_sort(intrusive_node *begin , intrusive_node *end, int(*cmp)(const intrusive_node *, const intrusive_node *)) {
  intrusive_node *edge = malloc(sizeof(intrusive_node));
  edge->prev = end->prev;
  edge->next = end;
  edge->prev->next = edge;
  edge->next->prev = edge;

  while (edge->prev != begin) {
    intrusive_node *to_proc = edge->prev;
    to_proc->next->prev = to_proc->prev;
    to_proc->prev->next = to_proc->next;

    intrusive_node *to_cmp = edge->next;
    for (; to_cmp != end; to_cmp = to_cmp->next) {
      if (cmp(to_cmp, to_proc) > 0) {
        break;
      }
    }
    // Insert the to_proc in front of to_cmp
    to_proc->next = to_cmp;
    to_proc->prev = to_cmp->prev;
    to_proc->next->prev = to_proc;
    to_proc->prev->next = to_proc;
  }

  edge->prev->next = edge->next;
  edge->next->prev = edge->prev;
  free(edge);
}


// Test
/* ————————————————————————————————————————————————————————————————————————
   下面开始编写一个单元测试函数 test_list_sort()，用来验证 list_sort 是否真的把一个
   含若干个“整型数据 + 链表节点” 的链表排序成升序，最后再把链表正确释放。
   ———————————————————————————————————————————————————————————————————————— */

/* 先定义一个“带数据域”的节点类型，值为 int，然后嵌入一个 intrusive_node： */
typedef struct test_node {
  int value;
  intrusive_node node;
} test_node;

/* 比较函数：把 intrusive_node 恢复回 test_node，再比较 value */
int cmp_node(const intrusive_node *a, const intrusive_node *b) {
  const test_node *ta = CONTAINER_OF(test_node, node, (intrusive_node *)a);
  const test_node *tb = CONTAINER_OF(test_node, node, (intrusive_node *)b);
  /* 我们希望按 value 升序排列，如果 a->value < b->value, 返回负数；等于返回 0；大于返回正 */
  return (ta->value - tb->value);
}

/* 这个函数里做完整的插入、排序、遍历检查、再释放 */
void test_list_sort(void) {
  /* 1) 初始化一个空的双向链表（head/tail 都是哨兵，无实际数据） */
  doubly_linked_list list;
  INIT_LINKED_LIST(doubly_linked_list, &list);

  /* 2) 构造几个 test_node 节点（顺序不排序），然后插入到链表里。 */
  /*    假设我们要对 [3, 1, 2] 这三个值排序测试 */
  intrusive_node *n3 = MAKE_NODE(test_node, node);
  CONTAINER_OF(test_node, node, n3)->value = 3;
  /* 在 head 之后插入 n3：head -> n3 -> tail */
  INSERT_BEHIND(&list, list.head, n3);

  intrusive_node *n1 = MAKE_NODE(test_node, node);
  CONTAINER_OF(test_node, node, n1)->value = 1;
  /* 在 n3 后插入 n1：head -> n3 -> n1 -> tail */
  INSERT_BEHIND(&list, n3, n1);

  intrusive_node *n2 = MAKE_NODE(test_node, node);
  CONTAINER_OF(test_node, node, n2)->value = 2;
  /* 在 n1 后插入 n2：head -> n3 -> n1 -> n2 -> tail */
  INSERT_BEHIND(&list, n1, n2);

  /* 此时链表中实际存的数据节点值序列：3, 1, 2 */

  assert(list.size == 3);

  /* 3) 调用 list_sort，把 head/tail 之间的所有“实际节点”排成升序 */
  list_sort(list.head, list.tail, cmp_node);

  /* 4) 排序完成后，依次从 head->next 开始往后遍历，检查值是 1, 2, 3 */
  intrusive_node *it = list.head->next;  /* 第一个实际节点 */
  int expect = 1;
  while (it != list.tail) {
    test_node *tn = CONTAINER_OF(test_node, node, it);
    /* 必须是升序 */
    assert(tn->value == expect);
    expect++;
    it = it->next;
  }
  /* expect 应该走到 4，说明刚好检查完 1, 2, 3 */
  assert(expect == 4);

  /* 5) 把链表里的所有节点都释放掉，然后释放哨兵 */
  DESTROY_LIST(test_node, node, doubly_linked_list, &list);

  /* 到这里，如果所有 assert 都没出错，就说明排序及释放都正常 */
  printf("[test_list_sort] passed.\n");
}

int main(void) {
  test_list_sort();
  return 0;
}
