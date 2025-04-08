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

// Begin and end does not store information.
void linked_list_sort(intrusive_node *begin, intrusive_node *end,
                      int (*cmp)(const intrusive_node *a,
                                 const intrusive_node *b)) {
  if (begin->next == end) {
    return;
  }

  size_t list_len = 0;
  for (intrusive_node *it = begin->next; it != end; it = it->next) {
    list_len++;
  }
  if (list_len == 1) {
    return;
  }

  size_t front_len = list_len / 2, back_len = list_len - (list_len / 2);
  // Insert the guard node to the middle of the list.
  intrusive_node guard_node, *iter = begin;
  for (size_t i = 0; i < front_len; i++) {
    iter = iter->next;
  }
  // Insert node to the back of the iterator.
  guard_node.next = iter->next, guard_node.prev = iter,
  iter->next->prev = &guard_node, iter->next = &guard_node;

  linked_list_sort(begin, &guard_node, cmp);
  linked_list_sort(&guard_node, end, cmp);

  intrusive_node new_begin, new_end;
  new_begin.prev = NULL, new_end.next = NULL, new_begin.next = &new_end,
  new_end.prev = &new_begin;
  intrusive_node *front_it = begin->next, *back_it = guard_node.next;

  while (front_it != &guard_node && back_it != end) {
    if (cmp(front_it, back_it) > 0) {
      intrusive_node *node_to_move = back_it;
      back_it = back_it->next;
      node_to_move->next = &new_end;
      node_to_move->prev = new_end.prev;
      new_end.prev->next = node_to_move;
      new_end.prev = node_to_move;
    } else {
      intrusive_node *node_to_move = front_it;
      front_it = front_it->next;
      node_to_move->next = &new_end;
      node_to_move->prev = new_end.prev;
      new_end.prev->next = node_to_move;
      new_end.prev = node_to_move;
    }
  }

  while (front_it != &guard_node) {
    intrusive_node *node_to_move = front_it;
    front_it = front_it->next;
    node_to_move->next = &new_end;
    node_to_move->prev = new_end.prev;
    new_end.prev->next = node_to_move;
    new_end.prev = node_to_move;
  }
  while (back_it != end) {
    intrusive_node *node_to_move = back_it;
    back_it = back_it->next;
    node_to_move->next = &new_end;
    node_to_move->prev = new_end.prev;
    new_end.prev->next = node_to_move;
    new_end.prev = node_to_move;
  }

  new_begin.next->prev = begin, new_end.prev->next = end;
  begin->next = new_begin.next, end->prev = new_end.prev;
}

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

// Specific program.

typedef doubly_linked_list linked_string;

typedef struct char_node {
  char dat;
  intrusive_node node;
} char_node;

int char_node_cmp(const intrusive_node *a, const intrusive_node *b) {
  return CONTAINER_OF(char_node, node, a)->dat -
         CONTAINER_OF(char_node, node, b)->dat;
}

void removeSubString(doubly_linked_list *src, const doubly_linked_list *sub) {
  for (auto iter = src->head->next; iter != src->tail;) {
    if (!char_node_cmp(iter, sub->head->next)) {
      auto tmpSrcIt = iter, tmpSubIt = sub->head->next;
      size_t matchSize = 0;
      while (tmpSrcIt != src->tail && tmpSubIt != sub->tail &&
             !char_node_cmp(tmpSrcIt, tmpSubIt)) {
        matchSize++;
        tmpSubIt = tmpSubIt->next;
        tmpSrcIt = tmpSrcIt->next;
      }
      if (matchSize == sub->size) {
        iter = tmpSrcIt;
        for (size_t i = 0; i < matchSize; i++) {
          REMOVE_AND_RELEASE(char_node, node, src, iter->prev);
        }
        if (iter == src->tail) {
          break;
        }
      } else {
        iter = iter->next;
      }
    } else {
      iter = iter->next;
    }
  }
}

int main() {
  linked_string src, sub;
  INIT_LINKED_LIST(doubly_linked_list, &src);
  INIT_LINKED_LIST(doubly_linked_list, &sub);

  intrusive_node *node;
  node = MAKE_NODE(char_node, node);
  CONTAINER_OF(char_node, node, node)->dat = 'a';
  INSERT_IN_FRONT_OF(&src, src.tail, node);
  node = MAKE_NODE(char_node, node);
  CONTAINER_OF(char_node, node, node)->dat = 'a';
  INSERT_IN_FRONT_OF(&src, src.tail, node);
  node = MAKE_NODE(char_node, node);
  CONTAINER_OF(char_node, node, node)->dat = 'b';
  INSERT_IN_FRONT_OF(&src, src.tail, node);
  node = MAKE_NODE(char_node, node);
  CONTAINER_OF(char_node, node, node)->dat = 'c';
  INSERT_IN_FRONT_OF(&src, src.tail, node);
  node = MAKE_NODE(char_node, node);
  CONTAINER_OF(char_node, node, node)->dat = 'a';
  INSERT_IN_FRONT_OF(&src, src.tail, node);
  node = MAKE_NODE(char_node, node);
  CONTAINER_OF(char_node, node, node)->dat = 'b';
  INSERT_IN_FRONT_OF(&src, src.tail, node);
  node = MAKE_NODE(char_node, node);
  CONTAINER_OF(char_node, node, node)->dat = 'c';
  INSERT_IN_FRONT_OF(&src, src.tail, node);
  node = MAKE_NODE(char_node, node);
  CONTAINER_OF(char_node, node, node)->dat = 'd';
  INSERT_IN_FRONT_OF(&src, src.tail, node);

  node = MAKE_NODE(char_node, node);
  CONTAINER_OF(char_node, node, node)->dat = 'a';
  INSERT_IN_FRONT_OF(&sub, sub.tail, node);
  node = MAKE_NODE(char_node, node);
  CONTAINER_OF(char_node, node, node)->dat = 'b';
  INSERT_IN_FRONT_OF(&sub, sub.tail, node);
  node = MAKE_NODE(char_node, node);
  CONTAINER_OF(char_node, node, node)->dat = 'c';
  INSERT_IN_FRONT_OF(&sub, sub.tail, node);

  removeSubString(&src, &sub);
  printf("%ld\n", src.size);
  for (auto iter = src.head->next; iter != src.tail; iter = iter->next) {
    putchar(CONTAINER_OF(char_node, node, iter)->dat);
  }
  putchar('\n');
}
