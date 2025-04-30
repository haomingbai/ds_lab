#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct node {
  char dat;
  struct node *next;
} node;

void delxallI(node *p, char x) {
  if (p->next == NULL) {
    return;
  }
  if (p->next->dat == x) {
    node *tmp = p->next->next;
    p->next = tmp;
    delxallI(p, x);
  } else {
    delxallI(p->next, x);
  }
}

void delxall(node *p, char x) {
  if (p == NULL) {
    return;
  }
  while (p->dat == x) {
    node *rhead = p->next;
    p->next = NULL;
    p = rhead;
    if (p == NULL) {
      return;
    }
  }
  delxallI(p, x);
  return;
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
  delxall(head, 'c');
  it = head;
  while (it != NULL) {
    putchar(it->dat);
    it = it->next;
  }
  putchar('\n');
}
