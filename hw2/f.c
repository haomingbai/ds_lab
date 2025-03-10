#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct _IntNode {
  int dat;
  struct _IntNode *prev, *next;
} IntNode;

void simpleInsertIntToNext(IntNode *node, int num);

IntNode *initBadList() {
  IntNode *head = malloc(sizeof(IntNode));
  head->next = head;
  head->prev = NULL;
  simpleInsertIntToNext(head, 2);
  simpleInsertIntToNext(head, 3);
  simpleInsertIntToNext(head, 4);
  simpleInsertIntToNext(head, 5);
  simpleInsertIntToNext(head, 6);
  simpleInsertIntToNext(head, 7);
  simpleInsertIntToNext(head, 8);
  simpleInsertIntToNext(head, 9);
  simpleInsertIntToNext(head, 10);
  return head;
}

void simpleInsertIntToNext(IntNode *node, int num) {
  IntNode *newNode = malloc(sizeof(IntNode));
  newNode->next = node->next;
  node->next = newNode;
  newNode->dat = num;
  newNode->prev = NULL;
}

void repairBadIntList(IntNode *head) {
  IntNode *prevNode = head, *nextNode = head->next;
  nextNode->prev = prevNode;
  prevNode = prevNode->next;
  nextNode = nextNode->next;
  while (prevNode != head) {
    nextNode->prev = prevNode;
    prevNode = prevNode->next;
    nextNode = nextNode->next;
  }
}

void doubleSidePrintIntList(IntNode *head) {
  for (IntNode *node = head->next; node != head; node = node->next) {
    printf("%d", node->dat);
    if (node->next != head) {
      printf(", ");
    }
  }
  putchar('\n');
  for (IntNode *node = head->prev; node != head; node = node->prev) {
    printf("%d", node->dat);
    if (node->prev != head) {
      printf(", ");
    }
  }
  putchar('\n');
}

int main() {
  IntNode *head = initBadList();
  repairBadIntList(head);
  doubleSidePrintIntList(head);
}
