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

typedef struct circular_linked_list {
  intrusive_node *head;
  size_t size;
} circular_linked_list;

void _init_circular_linked_list_(circular_linked_list *list_ptr);

void _free_empty_circular_linked_list_(circular_linked_list *list_ptr);

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

// char_node, a kind of node of char.
typedef struct char_node {
  char dat;
  intrusive_node node;
} char_node;

bool checkString(const char *str) {
  size_t len = strlen(str);

  if (!(len % 2)) {
    return false;
  }

  size_t lenOfX = len / 2;
  linked_queue queue;
  init_linked_queue(&queue);
  bool res = true;

  if (str[lenOfX] != '@') {
    res = false;
    goto free_and_return;
  }

  for (size_t i = 0; i < lenOfX; i++) {
    char ch = str[i];
    intrusive_node *nodePtr = MAKE_NODE(char_node, node);
    CONTAINER_OF(char_node, node, nodePtr)->dat = str[i];
    linked_queue_push(&queue, nodePtr);
  }

  for (size_t i = lenOfX + 1; i < len; i++) {
    char ch = str[i];
    intrusive_node *node = linked_queue_pop(&queue);
    if (CONTAINER_OF(char_node, node, node)->dat != ch) {
      free(CONTAINER_OF(char_node, node, node));
      res = false;
      goto free_and_return;
    }
    free(CONTAINER_OF(char_node, node, node));
  }

free_and_return:
  DESTROY_LIST(char_node, node, linked_queue, &queue);
  return res;
}

int main(void) {
  char str[1000] = {0};
  scanf("%s", str);

  bool flag = checkString(str);

  flag ? printf("true\n") : printf("false\n");
}
