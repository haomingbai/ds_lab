#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct node {
  char dat;
  struct node *next;
} node;

node *delxI(node *p, char x) {
  if (p->next == NULL) {
    return NULL;
  }
  if (p->next->dat == x) {
    node *res = p->next;
    p->next = p->next->next;
    return res;
  }
  return delxI(p->next, x);
}

struct delx_result {
  node *head, *deleted;
};
struct delx_result delx(node *p, char x) {
  if (p == NULL) {
    return (struct delx_result){NULL, NULL};
  }
  if (p->dat == x) {
    node *rhead = p->next;
    p->next = NULL;
    return (struct delx_result){rhead, p};
  }
  return (struct delx_result){p, delxI(p, x)};
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
  struct delx_result res = delx(head, 'c');
  it = head;
  while (it != NULL) {
    putchar(it->dat);
    it = it->next;
  }
  putchar('\n');
}
