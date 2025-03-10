#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define container_of(_nodePtr_, _nodeType_, _member_) \
  ((_nodeType_ *)((char *)(_nodePtr_) - (offsetof(_nodeType_, _member_))))

typedef struct _IntrusiveNode {
  struct _IntrusiveNode *next;
} IntrusiveNode;

typedef struct _IntNode {
  int dat;
  IntrusiveNode node;
} IntNode;

IntrusiveNode *initIntList() {
  IntNode *oriNode = (IntNode *)malloc(sizeof(IntNode));
  oriNode->node.next = NULL;
  return &oriNode->node;
}

void insertTo(IntrusiveNode *dest, IntrusiveNode *src) {
  src->next = dest->next;
  dest->next = src;
}

void insertIntTo(IntrusiveNode *dest, int num) {
  IntNode *node = (IntNode *)malloc(sizeof(IntNode));
  node->dat = num;
  insertTo(dest, &node->node);
}

void sortIntList(IntrusiveNode *head, size_t length) {
  // Fetch the length of the list
  if (length == 0) {
    IntrusiveNode *tmpHead = head;
    while (tmpHead->next != NULL) {
      tmpHead = tmpHead->next;
      length++;
    }
  }

  // End state
  if (length && length == 1) {
    return;
  }

  // Divide and conqure
  IntrusiveNode *mid = head;
  for (size_t i = 0; i < length / 2; i++) {
    mid = mid->next;
  }
  sortIntList(head, length / 2);
  sortIntList(mid, length - length / 2);

  // Merge the two sorted lists.
  IntrusiveNode *frontPtr = head, *backPtr = mid;
  while (frontPtr != mid && backPtr->next != NULL) {
    IntNode *front = container_of(frontPtr->next, IntNode, node),
            *back = container_of(backPtr->next, IntNode, node);
    if (front->dat > back->dat) {
      backPtr->next = backPtr->next->next;
      insertTo(frontPtr, &back->node);
      frontPtr = frontPtr->next;
    } else {
      frontPtr = frontPtr->next;
    }
  }
}

void printIntList(IntrusiveNode *head) {
  for (IntrusiveNode *node = head->next; node != NULL; node = node->next) {
    printf("%d", container_of(node, IntNode, node)->dat);
    if (node->next != NULL) {
      printf(", ");
    }
  }
  putchar('\n');
}

void freeIntList(IntrusiveNode *head) {
  IntrusiveNode *node = head;
  while (node != NULL) {
    IntrusiveNode *oldNode = node;
    node = node->next;
    free(oldNode);
  }
}

IntrusiveNode *subtractionAlgorithm(IntrusiveNode *firstList,
                                    IntrusiveNode *secondList) {
  sortIntList(firstList, 0);
  sortIntList(secondList, 0);

  IntrusiveNode *mergeList = initIntList();
  IntrusiveNode *mergeNode = mergeList, *firstNode = firstList->next,
                *secondNode = secondList->next;

  while (firstNode != NULL && secondNode != NULL) {
    IntNode *first = container_of(firstNode, IntNode, node),
            *second = container_of(secondNode, IntNode, node);
    if (first->dat == second->dat) {
      int val = first->dat;
      insertIntTo(mergeNode, val);
      mergeNode = mergeNode->next;
      while (firstNode && first->dat == val) {
        firstNode = firstNode->next;
        first = container_of(firstNode, IntNode, node);
      }
      while (secondNode && second->dat == val) {
        secondNode = secondNode->next;
        second = container_of(secondNode, IntNode, node);
      }
    } else if (first->dat > second->dat) {
      secondNode = secondNode->next;
    } else {
      firstNode = firstNode->next;
    }
  }

  IntrusiveNode *splitList = initIntList();
  IntrusiveNode *splitNode = splitList;
  firstNode = firstList->next;
  mergeNode = mergeList->next;
  while (firstNode != NULL && mergeNode != NULL) {
    IntNode *first = container_of(firstNode, IntNode, node),
            *merge = container_of(mergeNode, IntNode, node);
    if (merge->dat == first->dat) {
      int val = first->dat;
      while (firstNode && first->dat == val) {
        firstNode = firstNode->next;
        first = container_of(firstNode, IntNode, node);
      }
      while (mergeNode && merge->dat == val) {
        mergeNode = mergeNode->next;
        merge = container_of(secondNode, IntNode, node);
      }
    } else if (first->dat < merge->dat) {
      if (container_of(splitNode, IntNode, node)->dat != first->dat) {
        insertIntTo(splitNode, first->dat);
        splitNode = splitNode->next;
      }
      firstNode = firstNode->next;
    } else {
      mergeNode = mergeNode->next;
    }
  }

  while (firstNode != NULL) {
    IntNode *first = container_of(firstNode, IntNode, node);
    if (container_of(splitNode, IntNode, node)->dat != first->dat) {
      insertIntTo(splitNode, first->dat);
      splitNode = splitNode->next;
    }
    firstNode = firstNode->next;
  }

  return splitList;
}

int main() {
  IntrusiveNode *firstList = initIntList(), *secondList = initIntList();
  insertIntTo(firstList, 2);
  insertIntTo(firstList, 3);
  insertIntTo(firstList, 1);
  insertIntTo(secondList, 2);
  insertIntTo(secondList, 4);
  insertIntTo(secondList, 1);
  insertIntTo(secondList, 5);
  IntrusiveNode *resultList = subtractionAlgorithm(firstList, secondList);
  printIntList(resultList);
}
