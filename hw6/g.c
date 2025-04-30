#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct node {
  char dat;
  struct node *next;
} node;

node *maxNode(node *head) {
  if (head->next == NULL) {
    return head;
  }
  node *it = head->next;
  node *nr = maxNode(it);
  node *r = NULL;
  r = nr->dat > head->dat ? nr : head;
  return r;
}

node *minNode(node *head) {
  if (head->next == NULL) {
    return head;
  }
  node *it = head->next;
  node *nr = minNode(it);
  node *r = NULL;
  r = ((nr->dat) < (head->dat)) ? nr : head;
  return r;
}

int main() {
  char a;
  scanf("%c", &a);
  node *head = malloc(sizeof(node));
  head->dat = a;
  node *it = head;
  while (1) {
    char a;
    scanf("%c", &a);
    if (isspace(a)) {
      break;
    }
    node *node = malloc(sizeof(struct node));
    node->dat = a;
    node->next = NULL;
    it->next = node;
    it = node;
  }
  char max = maxNode(head)->dat, min = minNode(head)->dat;
  printf("max: %c\nmin: %c", max, min);
}
