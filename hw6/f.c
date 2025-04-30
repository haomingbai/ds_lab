#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct node {
  char dat;
  struct node *next;
} node;

int find(node *head, char val) {
  if (head == NULL) {
    return -1;
  }
  if (head->dat == val) {
    return 0;
  }
  int x = find(head->next, val);
  if (x == -1) {
    return -1;
  } else {
    return x + 1;
  }
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
    node *node = malloc(sizeof(node));
    node->dat = a;
    node->next = NULL;
    it->next = node;
    it = node;
  }
  int x = find(head, 'c');
  printf("%d\n", x);
}
