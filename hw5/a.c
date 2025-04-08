#include <stdio.h>
#include "sequence_list.h"

typedef struct array_with_offset {
  sequence_list lst;
  size_t offset;
} array_with_offset;

#define INIT_ARRAY_WITH_OFFSET(TYPE, ARR_PTR)    \
  do {                                           \
    INIT_SEQUENCE_LIST(TYPE, &((ARR_PTR)->lst)); \
    ((ARR_PTR)->offset) = 0;                     \
  } while (0)

#define ARRAY_WITH_OFFSET_REFERENCE(TYPE, ARR_PTR, POS) \
  SEQUENCE_LIST_REFERENCE(TYPE, (&((ARR_PTR)->lst)),    \
                          ((POS) + ((ARR_PTR)->offset)))

#define ARRAY_WITH_OFFSET_AT(TYPE, ARR_PTR, POS) \
  SEQUENCE_LIST_AT(TYPE, (&((ARR_PTR)->lst)),    \
                          ((POS) + ((ARR_PTR)->offset)))

#define ARRAY_WITH_OFFSET_INSERT(TYPE, ARR_PTR, POS, VAL) \
  SEQUENCE_LIST_INSERT(TYPE, (&((ARR_PTR)->lst)), ((POS) + ((ARR_PTR)->offset)), VAL)

// Test code

int main(void) {
  array_with_offset arr;
  INIT_ARRAY_WITH_OFFSET(int, &arr);
  ARRAY_WITH_OFFSET_INSERT(int, &arr, 0, 1);
  ARRAY_WITH_OFFSET_INSERT(int, &arr, 1, 2);
  ARRAY_WITH_OFFSET_INSERT(int, &arr, 2, 3);
  ARRAY_WITH_OFFSET_INSERT(int, &arr, 3, 4);
  ARRAY_WITH_OFFSET_INSERT(int, &arr, 4, 5);
  // 右移操作 ...
  size_t m = 2;
  arr.offset = m;
  for (size_t i = 0; i < 5; i++) {
    printf("%d\n", ARRAY_WITH_OFFSET_REFERENCE(int, &arr, i));
  }
}
