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

// Test code ...

// char_node, a kind of node of char.
typedef struct char_node {
  char dat;
  intrusive_node node;
} char_node;

// int main() {
//   doubly_linked_list dlist;
//   INIT_LINKED_LIST(doubly_linked_list, &dlist);
//   for (char i = 'a'; i < 'z'; i++) {
//     intrusive_node *newNode = MAKE_NODE(char_node, node);
//     CONTAINER_OF(char_node, node, newNode)->dat = i;
//     INSERT_IN_FRONT_OF(&dlist, dlist.tail, newNode);
//   }
//
//   intrusive_node *nodeForTest = dlist.head->next;
//
//   while (nodeForTest != dlist.tail) {
//     char x = CONTAINER_OF(char_node, node, nodeForTest)->dat;
//     putchar(x);
//     intrusive_node *oldNode = nodeForTest;
//     nodeForTest = nodeForTest->next;
//     // REMOVE_AND_RELEASE(char_node, node, &dlist, oldNode);
//   }
//   DESTROY_LIST(char_node, node, doubly_linked_list, &dlist);
//   putchar('\n');
// }

// int main() {
//   circular_linked_list clist;
//   INIT_LINKED_LIST(circular_linked_list, &clist);
//
//   for (char i = 'a'; i < 'z'; i++) {
//     intrusive_node *node = MAKE_NODE(char_node, node);
//     CONTAINER_OF(char_node, node, node)->dat = i;
//     INSERT_IN_FRONT_OF(&clist, clist.head, node);
//   }
//
//   intrusive_node *node = clist.head->next;
//
//   while(node != clist.head) {
//     char x = CONTAINER_OF(char_node, node, node)->dat;
//     putchar(x);
//     node = node->next;
//   }
//
//   DESTROY_LIST(char_node, node, circular_linked_list, &clist);
// }

// Linked queue.

typedef circular_linked_list linked_queue;

void init_linked_queue(linked_queue *queue) {
  INIT_LINKED_LIST(circular_linked_list, queue);
}

bool linked_queue_empty(linked_queue *queue) { return IS_EMPTY(queue); }

void linked_queue_push(linked_queue *queue, intrusive_node *node) {
  INSERT_BEHIND(queue, queue->head, node);
}

const intrusive_node *linked_queue_front(linked_queue *queue) {
  return queue->head->prev;
}

intrusive_node *linked_queue_pop(linked_queue *queue) {
  intrusive_node *node = REMOVE_NODE_FROM_LIST(queue, queue->head->prev);
  return node;
}

#define _free_empty_linked_queue_ _free_empty_circular_linked_list_

void moveDigitToFront(char *str) {
  size_t len = strlen(str);
  linked_queue qNum, qChar;
  init_linked_queue(&qNum);
  init_linked_queue(&qChar);

  for (size_t i = 0; i < len; i++) {
    if ('0' <= str[i] && str[i] <= '9') {
      intrusive_node *newNode = MAKE_NODE(char_node, node);
      CONTAINER_OF(char_node, node, newNode)->dat = str[i];
      linked_queue_push(&qNum, newNode);
    } else if ('a' <= str[i] && str[i] <= 'z') {
      intrusive_node *newNode = MAKE_NODE(char_node, node);
      CONTAINER_OF(char_node, node, newNode)->dat = str[i];
      linked_queue_push(&qChar, newNode);
    } else {
      assert(0);
    }
  }

  size_t idx = 0;

  while (!linked_queue_empty(&qNum)) {
    intrusive_node *node = linked_queue_pop(&qNum);
    char ch = CONTAINER_OF(char_node, node, node)->dat;
    free(CONTAINER_OF(char_node, node, node));
    str[idx++] = ch;
  }

  while (!linked_queue_empty(&qChar)) {
    intrusive_node *node = linked_queue_pop(&qChar);
    char ch = CONTAINER_OF(char_node, node, node)->dat;
    free(CONTAINER_OF(char_node, node, node));
    str[idx++] = ch;
  }

  DESTROY_LIST(char_node, node, linked_queue, &qNum);
  DESTROY_LIST(char_node, node, linked_queue, &qChar);
}

int main() {
  size_t n;
  scanf("%ld", &n);

  char *str = (char *)(malloc(sizeof(char) * (n + 1)));
  scanf("%s", str);

  printf("original string: %s\n", str);
  moveDigitToFront(str);
  printf("processed string: %s\n", str);
}
