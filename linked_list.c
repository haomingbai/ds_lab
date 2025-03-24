#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define CONTAINER_OF(NODE_TYPE, MEMBER, NODE_PTR) \
  ((NODE_TYPE *)((char *)(NODE_PTR) - (offsetof(NODE_TYPE, MEMBER))))

#define MAKE_NODE(NODE_TYPE, MEMBER) \
  ((IntrusiveNode *)(&(((NODE_TYPE *)(malloc(sizeof(NODE_TYPE))))->node)))

#define REMOVE_NODE_FROM_LIST(LIST_PTR, NODE_PTR) \
  (((LIST_PTR)->size--), _remove_Node_WithoutChangingSize(NODE_PTR));

#define REMOVE_AND_RELEASE(NODE_TYPE, MEMBER, LIST_PTR, NODE_PTR) \
  do {                                                            \
    IntrusiveNode *_NODE_TO_DEL = NODE_PTR;                       \
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
    _freeEmpty_##LIST_TYPE##_(LIST_PTR);                          \
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

#define INIT_LIST(LIST_TYPE, LIST_PTR) (_init_##LIST_TYPE##_(LIST_PTR))

typedef struct IntrusiveNode {
  struct IntrusiveNode *prev, *next;
} IntrusiveNode;

typedef struct DoublyLinkedList {
  IntrusiveNode *head, *tail;
  size_t size;
} DoublyLinkedList;

typedef struct CircularLinkedList {
  IntrusiveNode *head;
  size_t size;
} CircularLinkedList;

void _init_DoublyLinkedList_(DoublyLinkedList *listPtr);

void _init_CircularLinkedList_(CircularLinkedList *listPtr);

void _freeEmpty_DoublyLinkedList_(DoublyLinkedList *listPtr);

void _freeEmpty_CircularLinkedList_(CircularLinkedList *listPtr);

void _init_DoublyLinkedList_(DoublyLinkedList *listPtr) {
  IntrusiveNode *headNode = NULL, *tailNode = NULL;
  headNode = (IntrusiveNode *)malloc(sizeof(IntrusiveNode));
  tailNode = (IntrusiveNode *)malloc(sizeof(IntrusiveNode));

  if (headNode == 0 || tailNode == 0) {
    perror("Init doubly linked list failed!");
    free(tailNode);
    free(headNode);
    return;
  }

  headNode->next = tailNode;
  headNode->prev = NULL;
  tailNode->next = NULL;
  tailNode->prev = headNode;

  listPtr->size = 0;
  listPtr->head = headNode;
  listPtr->tail = tailNode;
}

void _init_CircularLinkedList_(CircularLinkedList *listPtr) {
  IntrusiveNode *headNode = 0;
  headNode = (IntrusiveNode *)malloc(sizeof(IntrusiveNode));

  if (headNode == 0) {
    perror("Init circular linked list failed!");
    free(headNode);
    return;
  }

  headNode->prev = headNode;
  headNode->next = headNode;

  listPtr->head = headNode;
  listPtr->size = 0;
}

void _freeEmpty_DoublyLinkedList_(DoublyLinkedList *listPtr) {
  assert(listPtr->size == 0);
  free(listPtr->tail);
  free(listPtr->head);
  listPtr->tail = NULL;
  listPtr->head = NULL;
}

IntrusiveNode * _remove_Node_WithoutChangingSize(IntrusiveNode *nodePtr) {
  nodePtr->prev->next = nodePtr->next;
  nodePtr->next->prev = nodePtr->prev;
  nodePtr->prev = 0;
  nodePtr->next = 0;
  return nodePtr;
}

void _freeEmpty_CircularLinkedList_(CircularLinkedList *listPtr) {
  assert(listPtr->size == 0);
  free(listPtr->head);
  listPtr->head = NULL;
}

// Test code ...

// CharNode, a kind of node of char.
typedef struct CharNode {
  char dat;
  IntrusiveNode node;
} CharNode;

// int main() {
//   DoublyLinkedList dlist;
//   INIT_LIST(DoublyLinkedList, &dlist);
//   for (char i = 'a'; i < 'z'; i++) {
//     IntrusiveNode *newNode = MAKE_NODE(CharNode, node);
//     CONTAINER_OF(CharNode, node, newNode)->dat = i;
//     INSERT_IN_FRONT_OF(&dlist, dlist.tail, newNode);
//   }
// 
//   IntrusiveNode *nodeForTest = dlist.head->next;
// 
//   while (nodeForTest != dlist.tail) {
//     char x = CONTAINER_OF(CharNode, node, nodeForTest)->dat;
//     putchar(x);
//     IntrusiveNode *oldNode = nodeForTest;
//     nodeForTest = nodeForTest->next;
//     // REMOVE_AND_RELEASE(CharNode, node, &dlist, oldNode);
//   }
//   DESTROY_LIST(CharNode, node, DoublyLinkedList, &dlist);
//   putchar('\n');
// }

// int main() {
//   CircularLinkedList clist;
//   INIT_LIST(CircularLinkedList, &clist);
// 
//   for (char i = 'a'; i < 'z'; i++) {
//     IntrusiveNode *node = MAKE_NODE(CharNode, node);
//     CONTAINER_OF(CharNode, node, node)->dat = i;
//     INSERT_IN_FRONT_OF(&clist, clist.head, node);
//   }
// 
//   IntrusiveNode *node = clist.head->next;
// 
//   while(node != clist.head) {
//     char x = CONTAINER_OF(CharNode, node, node)->dat;
//     putchar(x);
//     node = node->next;
//   }
// 
//   DESTROY_LIST(CharNode, node, CircularLinkedList, &clist);
// }
