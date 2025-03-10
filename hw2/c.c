#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ORI_PTR(intuitiveNodePtr, NodeType, nodeMember) \
  ((NodeType *)((char *)(intuitiveNodePtr) - (offsetof(NodeType, nodeMember))))

typedef struct _SimpleIntuitiveNode {
  struct _SimpleIntuitiveNode *next;
} SimpleIntuitiveNode;

typedef struct _VoidNode {
  SimpleIntuitiveNode node;
} VoidNode;

typedef struct _IntNode {
  int dat;
  SimpleIntuitiveNode node;
} IntNode;

SimpleIntuitiveNode *genList() {
  VoidNode *ptr = (VoidNode *)(malloc(sizeof(VoidNode)));
  ptr->node.next = NULL;
  return &(ptr->node);
}

bool insertToNextByIterator(SimpleIntuitiveNode *iterator,
                            SimpleIntuitiveNode *newNode) {
  newNode->next = iterator->next;
  iterator->next = newNode;
  return true;
}

SimpleIntuitiveNode *removeNextByIterator(SimpleIntuitiveNode *iterator) {
  SimpleIntuitiveNode *nextPtr = iterator->next;
  iterator->next = nextPtr->next;
  return nextPtr;
}

void proc(SimpleIntuitiveNode *head, int dat) {
  SimpleIntuitiveNode *node = head;
  while (node->next != NULL) {
    if (dat == (ORI_PTR(node->next, IntNode, node))->dat) {
      free(ORI_PTR(removeNextByIterator(node), IntNode, node));
      continue;
    }
    node = node->next;
  }
}

int main() {
  SimpleIntuitiveNode *head = genList();
  IntNode *a[5];
  for (size_t i = 0; i < 5; i++) {
    a[i] = malloc(sizeof(IntNode));
  }
  a[0]->dat = 0;
  a[1]->dat = 1;
  a[2]->dat = 3;
  a[3]->dat = 1;
  a[4]->dat = 2;
  SimpleIntuitiveNode *node = head;
  for (size_t i = 0; i < 5; i++) {
    insertToNextByIterator(node, &(a[i]->node));
    node = node->next;
  }
  proc(head, 1); // Remove all the nodes with dat == 1
  node = head;
  while (node->next != NULL) {
    printf("%d\n", (ORI_PTR(node->next, IntNode, node))->dat);
    node = node->next;
  }
}
