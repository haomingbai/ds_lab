#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

/**
 * @brief Macro to initialize a stack.
 *
 * This macro initializes a stack of a specified type.
 *
 * @param TYPE The data type of the stack elements.
 * @param STACK_PTR Pointer to the stack structure.
 */
#define INIT_STACK(TYPE, STACK_PTR)                       \
  do {                                                    \
    (STACK_PTR)->data = ((void *)(malloc(sizeof(TYPE)))); \
    (STACK_PTR)->capacity = 1;                            \
    (STACK_PTR)->size = 0;                                \
    (STACK_PTR)->inited = 1;                              \
  } while (0)

/**
 * @brief Macro to push a value onto a stack.
 *
 * @param TYPE The data type of the stack elements.
 * @param STACK_PTR Pointer to the stack.
 * @param VAL The value to be pushed onto the stack.
 */
#define STACK_PUSH(TYPE, STACK_PTR, VAL)                                       \
  do {                                                                         \
    if (((STACK_PTR)->size) == ((STACK_PTR)->capacity)) {                      \
      size_t _newCapacity =                                                    \
          ((((STACK_PTR)->capacity) * 2 > ((STACK_PTR)->size) + 1)             \
               ? ((STACK_PTR)->capacity * 2)                                   \
               : ((STACK_PTR)->size + 1));                                     \
      ((STACK_PTR)->data) =                                                    \
          ((void *)realloc(((STACK_PTR)->data), sizeof(TYPE) * _newCapacity)); \
      ((STACK_PTR)->capacity) = _newCapacity;                                  \
    }                                                                          \
    (((TYPE *)((STACK_PTR)->data))[((STACK_PTR)->size)]) = VAL;                \
    ((STACK_PTR)->size)++;                                                     \
  } while (0)

/**
 * @brief Macro to check if a stack is empty.
 *
 * This macro evaluates to true if the stack is empty, i.e., its size is zero.
 *
 * @param TYPE The data type of the stack elements.
 * @param STACK_PTR Pointer to the stack structure.
 * @return Non-zero value if the stack is empty, zero otherwise.
 */
#define STACK_TOP(TYPE, STACK_PTR) \
  (((TYPE *)((STACK_PTR)->data))[((STACK_PTR)->size) - 1])

/**
 * @brief Macro to pop an element from the stack.
 *
 * This macro removes the top element from the stack pointed to by STACK_PTR.
 *
 * @param STACK_PTR Pointer to the stack from which the element will be popped.
 */
#define STACK_POP(TYPE, STACK_PTR)                                             \
  do {                                                                         \
    (((STACK_PTR)->size)--);                                                   \
    if (((STACK_PTR)->size) < (((STACK_PTR)->capacity) >> 1)) {                \
      size_t _newCapacity = (((STACK_PTR)->capacity) >> 1);                    \
      ((STACK_PTR)->data) =                                                    \
          ((void *)realloc(((STACK_PTR)->data), sizeof(TYPE) * _newCapacity)); \
      ((STACK_PTR)->capacity) = _newCapacity;                                  \
    }                                                                          \
  } while (0)

/**
 * @brief Macro to shrink the size of a stack.
 *
 * This macro is used to reduce the size of a stack of a specified type.
 *
 * @param TYPE The data type of the elements in the stack.
 * @param STACK_PTR A pointer to the stack that needs to be shrunk.
 */
#define STACK_SHRINK(TYPE, STACK_PTR)                                       \
  do {                                                                      \
    if (((STACK_PTR)->size) < ((STACK_PTR)->capacity)) {                    \
      ((STACK_PTR)->capacity) = ((STACK_PTR)->size);                        \
      ((STACK_PTR)->data) = ((void *)(realloc(                              \
          ((STACK_PTR)->data), (sizeof(TYPE) * ((STACK_PTR)->capacity))))); \
    }                                                                       \
  } while (0)

/**
 * @brief Macro to check if a stack is empty.
 *
 * This macro evaluates to true if the stack is empty, i.e., its size is zero.
 *
 * @param STACK_PTR Pointer to the stack structure.
 * @return Non-zero value if the stack is empty, zero otherwise.
 */
#define STACK_EMPTY(STACK_PTR) ((STACK_PTR)->size == 0)

/**
 * @brief Macro to get the size of a stack.
 *
 * This macro evaluates to the number of elements in the stack.
 *
 * @param STACK_PTR Pointer to the stack structure.
 * @return The number of elements in the stack.
 */
#define STACK_SIZE(STACK_PTR) ((STACK_PTR)->size)

/**
 * @brief Macro to destroy a stack.
 *
 * This macro takes a pointer to a stack and performs the necessary
 * operations to destroy the stack, freeing any allocated resources.
 *
 * @param STACK_PTR Pointer to the stack to be destroyed.
 */
#define DESTROY_STACK(STACK_PTR) \
  do {                           \
    free(((STACK_PTR)->data));   \
    ((STACK_PTR)->inited) = 0;   \
    ((STACK_PTR)->size) = 0;     \
    ((STACK_PTR)->capacity) = 0; \
  } while (0)

/**
 * @file sequence_stack.c
 * @brief Implementation of a sequence stack data structure.
 *
 * This file contains the implementation of a sequence stack, which is a stack
 * data structure that uses a sequence (array) to store its elements.
 */
typedef struct SequenceStack {
  void *data;
  size_t capacity;
  size_t size;
  bool inited;
} SequenceStack;

// Test the stack implementation.
#include <stdio.h>

int main() {
  SequenceStack st;
  INIT_STACK(int, &st);
  for (int i = 0; i < 10; i++) {
    STACK_PUSH(int, &st, i);
  }
  while (!STACK_EMPTY(&st)) {
    printf("stack info: size: %ld, capacity: %ld\n", st.size, st.capacity);
    printf("The value on the top of the stack: %d\n", STACK_TOP(int, &st));
    STACK_POP(int, &st);
    // STACK_SHRINK(int, &st);
  }
}
