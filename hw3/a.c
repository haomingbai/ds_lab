#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STACK(Type) stack_##Type

#define GEN_STACK_STRUCT(Type)  \
  typedef struct stack_##Type { \
    Type *head;                 \
    size_t len;                 \
    size_t capacity;            \
  } stack_##Type

#define INIT_STACK(Type, stackPtr)                     \
  {                                                    \
    (stackPtr)->head = (Type *)(malloc(sizeof(Type))); \
    (stackPtr)->capacity = 1;                          \
    (stackPtr)->len = 0;                               \
  }

#define STACK_PUSH(Type, stackPtr, newElem)                                 \
  {                                                                         \
    if ((stackPtr)->len == (stackPtr)->capacity) {                          \
      size_t newCapacity = 2 * ((stackPtr)->capacity) > (stackPtr)->len + 1 \
                               ? (2 * ((stackPtr)->capacity))               \
                               : ((stackPtr)->len + 1);                     \
      (stackPtr)->capacity = newCapacity;                                   \
      (stackPtr)->head = (Type *)(realloc((void *)((stackPtr)->head),       \
                                          sizeof(Type) * (newCapacity)));   \
    }                                                                       \
    (stackPtr)->head[((stackPtr)->len)] = newElem;                          \
    ((stackPtr)->len)++;                                                    \
  }

#define STACK_TOP(stackPtr) ((stackPtr)->head[((stackPtr)->len) - 1])

#define STACK_POP(stackPtr) \
  {                         \
    if ((stackPtr)->len) {  \
      ((stackPtr)->len)--;  \
    }                       \
  }

#define STACK_SHRINK(Type, stackPtr)                                \
  {                                                                 \
    if (((stackPtr)->capacity) > ((stackPtr)->len)) {               \
      (stackPtr)->capacity = (stackPtr)->len;                       \
      (stackPtr)->head =                                            \
          (Type *)(realloc(sizeof(Type) * ((stackPtr)->capacity))); \
    }                                                               \
  }

#define IS_STACK_EMPTY(stackPtr) (!((stackPtr)->len))

#define RELEASE_STACK(stackPtr) \
  {                             \
    free((stackPtr)->head);           \
    (stackPtr)->len = 0;        \
    (stackPtr)->capacity = 0;   \
  }

GEN_STACK_STRUCT(char);

bool judge(const char *str) {
  size_t len = strlen(str);
  size_t currentPos = 0;
  if (len % 2 == 0) {
    return false;
  } else {
    STACK(char) st;
    INIT_STACK(char, &st);
    for (currentPos = 0; currentPos < (len - 1) / 2; currentPos++) {
      STACK_PUSH(char, &st, str[currentPos]);
    }
    if (str[currentPos] != '@') {
      return false;
    }
    currentPos++;
    for (; currentPos < len; currentPos++) {
      char curr = STACK_TOP(&st);
      STACK_POP(&st);
      if (str[currentPos] != curr) {
        RELEASE_STACK(&st);
        return false;
      }
    }
    RELEASE_STACK(&st);
    return true;
  }
}

int main() {
  char str[1000];
  scanf("%s", str);
  bool val = judge(str);
  val ? puts("true") : puts("false");
}
