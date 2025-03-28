#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_OF(A, B) (((A) < (B)) ? (B) : (A))

typedef struct sequence_list {
  void *data;
  size_t size;
  size_t capacity;
} sequence_list;

#define INIT_SEQUENCE_LIST(TYPE, LIST_PTR) \
  do {                                     \
    void *dat = malloc(sizeof(TYPE));      \
    if (dat == NULL) {                     \
      perror("Fail to init the list.");    \
    } else {                               \
      ((LIST_PTR)->capacity) = 1;          \
      ((LIST_PTR)->size) = 0;              \
      ((LIST_PTR)->data) = dat;            \
    }                                      \
  } while (0)

#define SEQUENCE_LIST_PUSH_BACK(TYPE, LIST_PTR, VAL)                  \
  do {                                                                \
    bool flag = true;                                                 \
    if (((LIST_PTR)->size) == ((LIST_PTR)->capacity)) {               \
      size_t new_capacity =                                           \
          MAX_OF(((LIST_PTR)->size) + 1, ((LIST_PTR)->capacity) * 2); \
      void *new_data =                                                \
          realloc(((LIST_PTR)->data), new_capacity * sizeof(TYPE));   \
      if (new_data == NULL) {                                         \
        perror("Fail to alloc extra memory, rollback...");            \
        flag = false;                                                 \
      } else {                                                        \
        ((LIST_PTR)->data) = new_data;                                \
        ((LIST_PTR)->capacity) = new_capacity;                        \
      }                                                               \
    }                                                                 \
    if (flag) {                                                       \
      ((TYPE *)((LIST_PTR)->data))[((LIST_PTR)->size)] = (TYPE)(VAL); \
      ((LIST_PTR)->size)++;                                           \
    }                                                                 \
  } while (0)

#define SEQUENCE_LIST_REFERENCE(TYPE, LIST_PTR, POSITION) \
  (((TYPE *)((LIST_PTR)->data))[(POSITION) % ((LIST_PTR)->size)])

#define SEQUENCE_LIST_POP_BACK(TYPE, LIST_PTR)                      \
  do {                                                              \
    ((LIST_PTR)->size)--;                                           \
    if (((LIST_PTR)->capacity) > (((LIST_PTR)->size) * 4 + 1)) {    \
      size_t new_capacity = MAX_OF(((LIST_PTR)->size) * 4 + 1, 1);  \
      void *new_data =                                              \
          realloc(((LIST_PTR)->data), new_capacity * sizeof(TYPE)); \
      if (new_data == NULL) {                                       \
        perror("Fail to alloc new memory, continue...");            \
      } else {                                                      \
        ((LIST_PTR)->data) = new_data;                              \
        ((LIST_PTR)->capacity) = new_capacity;                      \
      }                                                             \
    }                                                               \
  } while (0)

#define SEQUENCE_LIST_INSERT(TYPE, LIST_PTR, POS, VAL)                        \
  do {                                                                        \
    bool flag = true;                                                         \
    size_t old_size = (LIST_PTR)->size;                                       \
    size_t new_size = ((LIST_PTR)->size) + 1;                                 \
    if (new_size > ((LIST_PTR)->capacity)) {                                  \
      size_t new_capacity = MAX_OF((((LIST_PTR)->capacity) * 2), (new_size)); \
      void *new_data =                                                        \
          realloc(((LIST_PTR)->data), new_capacity * sizeof(TYPE));           \
      if (new_data == NULL) {                                                 \
        perror("Fail to alloc new memory, rollback...");                      \
        flag = false;                                                         \
      } else {                                                                \
        ((LIST_PTR)->data) = new_data;                                        \
        ((LIST_PTR)->capacity) = new_capacity;                                \
      }                                                                       \
    }                                                                         \
    if (flag) {                                                               \
      ((LIST_PTR)->size) = new_size;                                          \
      TYPE *data = ((TYPE *)((LIST_PTR)->data));                              \
      memmove((void *)((TYPE *)(data) + ((POS) + 1)),                         \
              ((void *)((TYPE *)(data) + (POS))), ((old_size) - (POS)));      \
      data[(POS)] = (VAL);                                                    \
    }                                                                         \
  } while (0)

#define SEQUENCE_LIST_REMOVE(TYPE, LIST_PTR, POS)                   \
  do {                                                              \
    memmove(((void *)(((TYPE *)((LIST_PTR)->data)) + (POS))),       \
            ((void *)(((TYPE *)((LIST_PTR)->data) + (POS) + 1))),   \
            (((LIST_PTR)->size) - 1 - (POS)));                      \
    ((LIST_PTR)->size)--;                                           \
    if (((LIST_PTR)->capacity) > (((LIST_PTR)->size) * 4 + 1)) {    \
      size_t new_capacity = MAX_OF(((LIST_PTR)->size) * 4 + 1, 1);  \
      void *new_data =                                              \
          realloc(((LIST_PTR)->data), new_capacity * sizeof(TYPE)); \
      if (new_data == NULL) {                                       \
        perror("Fail to alloc new memory, continue...");            \
      } else {                                                      \
        ((LIST_PTR)->data) = new_data;                              \
        ((LIST_PTR)->capacity) = new_capacity;                      \
      }                                                             \
    }                                                               \
  } while (0)

#define SEQUENCE_LIST_RESERVE(TYPE, LIST_PTR, NEW_SIZE)                        \
  do {                                                                         \
    size_t new_capacity = (NEW_SIZE);                                          \
    void *new_data = realloc(((LIST_PTR)->data), new_capacity * sizeof(TYPE)); \
    if (new_data == NULL) {                                                    \
      perror("Fail to alloc new memory, rollback...");                         \
    } else {                                                                   \
      ((LIST_PTR)->data) = new_data;                                           \
      ((LIST_PTR)->capacity) = new_capacity;                                   \
    }                                                                          \
  } while (0)

#define SEQUENCE_LIST_RESIZE(TYPE, LIST_PTR, NEW_SIZE)              \
  do {                                                              \
    bool flag = true;                                               \
    if ((NEW_SIZE) >= ((LIST_PTR)->capacity)) {                     \
      size_t new_capacity =                                         \
          MAX_OF((NEW_SIZE) + 1, ((LIST_PTR)->capacity) * 2);       \
      void *new_data =                                              \
          realloc(((LIST_PTR)->data), new_capacity * sizeof(TYPE)); \
      if (new_data == NULL) {                                       \
        perror("Fail to alloc extra memory, rollback...");          \
        flag = false;                                               \
      } else {                                                      \
        ((LIST_PTR)->data) = new_data;                              \
        ((LIST_PTR)->capacity) = new_capacity;                      \
      }                                                             \
    }                                                               \
    if (flag) {                                                     \
      ((LIST_PTR)->size) = (NEW_SIZE);                              \
    }                                                               \
  } while (0)

#define SEQUENCE_LIST_SHRINK(TYPE, LIST_PTR)                                   \
  do {                                                                         \
    size_t new_capacity = MAX_OF(((LIST_PTR)->size), 1);                       \
    void *new_data = realloc(((LIST_PTR)->data), new_capacity * sizeof(TYPE)); \
    if (new_data == NULL) {                                                    \
      perror("Fail to alloc new memory, continue...");                         \
    } else {                                                                   \
      ((LIST_PTR)->data) = new_data;                                           \
      ((LIST_PTR)->capacity) = new_capacity;                                   \
    }                                                                          \
  } while (0)

void destroy_sequence_list(sequence_list *lst) {
  free(lst->data);
  lst->capacity = 0;
  lst->size = 0;
}

bool sequence_list_empty(sequence_list *lst) { return lst->size == 0; }

// Matrix

typedef struct matrix {
  sequence_list data;
  size_t row_num;
  size_t col_num;
} matrix;

#define INIT_MATRIX(TYPE, MAT_PTR, ROW_NUM, COL_NUM)                  \
  do {                                                                \
    size_t total_size = ((ROW_NUM) * (COL_NUM));                      \
    INIT_SEQUENCE_LIST(TYPE, &((MAT_PTR)->data));                     \
    SEQUENCE_LIST_RESIZE(TYPE, &((MAT_PTR)->data), total_size);                  \
    ((MAT_PTR)->row_num) = ROW_NUM;                                   \
    ((MAT_PTR)->col_num) = COL_NUM;                                   \
    memset((((MAT_PTR)->data).data), 0, (sizeof(TYPE) * total_size)); \
  } while (0)

#define MATRIX_REFERENCE(TYPE, MAT_PTR, ROW, COL)                            \
  (((TYPE *)($((MAT_PTR)->data)))[((((MAT_PTR)->col_num) * (ROW)) + (COL)) * \
                                  sizeof(TYPE)])

void destroy_matrix(matrix *mat) {
  destroy_sequence_list(&mat->data);
  mat->col_num = 0;
  mat->row_num = 0;
}

int main() {
  matrix mat;
  INIT_MATRIX(double, &mat, 10, 10);
}
