#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Sequence list data structure

#define MAX_OF(A, B) (((A) < (B)) ? (B) : (A))

typedef struct sequence_list {
  void *data;
  size_t size;
  size_t capacity;
} sequence_list;

size_t sequence_list_size(sequence_list *lst) { return lst->size; }

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

#define SEQUENCE_LIST_AT(TYPE, LIST_PTR, POSITION) \
  (((TYPE *)((LIST_PTR)->data)) + (POSITION))

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

// Matrix data structure

#define DEFINE_MAT_ELEM_STRUCT(TYPE)                                  \
  typedef struct mat_elem_##TYPE {                                    \
    size_t i;                                                         \
    size_t j;                                                         \
    TYPE elem;                                                        \
  } mat_elem_##TYPE;                                                  \
  int mat_elem_##TYPE##_val_cmp(const void *a, const void *b) {       \
    const mat_elem_##TYPE *aPtr = (mat_elem_##TYPE *)a,               \
                          *bPtr = (mat_elem_##TYPE *)b;               \
    if (aPtr->elem > bPtr->elem) {                                    \
      return 1;                                                       \
    } else if (aPtr->elem == bPtr->elem) {                            \
      return 0;                                                       \
    } else {                                                          \
      return -1;                                                      \
    }                                                                 \
  }                                                                   \
  int mat_elem_##TYPE##_row_first_cmp(const void *a, const void *b) { \
    const mat_elem_##TYPE *aPtr = (mat_elem_##TYPE *)a,               \
                          *bPtr = (mat_elem_##TYPE *)b;               \
    if (aPtr->i > bPtr->i) {                                          \
      return 1;                                                       \
    } else if (aPtr->i < bPtr->i) {                                   \
      return -1;                                                      \
    } else {                                                          \
      if (aPtr->j > bPtr->j) {                                        \
        return 1;                                                     \
      } else if (aPtr->j < bPtr->j) {                                 \
        return -1;                                                    \
      } else {                                                        \
        return 0;                                                     \
      }                                                               \
    }                                                                 \
  }                                                                   \
  int mat_elem_##TYPE##_col_first_cmp(const void *a, const void *b) { \
    const mat_elem_##TYPE *aPtr = (mat_elem_##TYPE *)a,               \
                          *bPtr = (mat_elem_##TYPE *)b;               \
    if (aPtr->j > bPtr->j) {                                          \
      return 1;                                                       \
    } else if (aPtr->j < bPtr->j) {                                   \
      return -1;                                                      \
    } else {                                                          \
      if (aPtr->i > bPtr->i) {                                        \
        return 1;                                                     \
      } else if (aPtr->i < bPtr->i) {                                 \
        return -1;                                                    \
      } else {                                                        \
        return 0;                                                     \
      }                                                               \
    }                                                                 \
  }

#define MAT_ELEM(TYPE) mat_elem_##TYPE
#define MAT_ELEM_CMP(TYPE, CMP_TYPE) mat_elem_##TYPE##_##CMP_TYPE##_cmp

enum seq_type { ROW, COL, VAL, MASS };

typedef struct matrix {
  // The data of elements.
  sequence_list data;
  size_t row_num, col_num;
} matrix;

#define INIT_MATRIX(TYPE, MAT_PTR, ROW_NUM, COL_NUM)          \
  do {                                                        \
    INIT_SEQUENCE_LIST(MAT_ELEM(TYPE), (&((MAT_PTR)->data))); \
    ((MAT_PTR)->row_num) = (ROW_NUM);                         \
    ((MAT_PTR)->col_num) = (COL_NUM);                         \
  } while (0)

#define MATRIX_ADD_ELEM(TYPE, MAT_PTR, ELEM)                               \
  do {                                                                     \
    SEQUENCE_LIST_PUSH_BACK(MAT_ELEM(TYPE), (&((MAT_PTR)->data)), (ELEM)); \
  } while (0)

#define MATRIX_ELEM_NUM(MAT_PTR) (sequence_list_size((&((MAT_PTR)->data))))

#define MATRIX_SET_SEQ(TYPE, MAT_PTR, SEQ_TYPE)                       \
  do {                                                                \
    switch ((SEQ_TYPE)) {                                             \
      case ROW:                                                       \
        qsort(((MAT_PTR)->data.data), MATRIX_ELEM_NUM((MAT_PTR)),     \
              sizeof(MAT_ELEM(TYPE)), MAT_ELEM_CMP(TYPE, row_first)); \
        break;                                                        \
      case COL:                                                       \
        qsort(((MAT_PTR)->data.data), MATRIX_ELEM_NUM((MAT_PTR)),     \
              sizeof(MAT_ELEM(TYPE)), MAT_ELEM_CMP(TYPE, col_first)); \
        break;                                                        \
      case VAL:                                                       \
        qsort(((MAT_PTR)->data.data), MATRIX_ELEM_NUM((MAT_PTR)),     \
              sizeof(MAT_ELEM(TYPE)), MAT_ELEM_CMP(TYPE, val));       \
        break;                                                        \
      default:                                                        \
        break;                                                        \
    }                                                                 \
  } while (0)

#define MATRIX_TRANSPOSE(TYPE, MAT_PTR, SEQ_TYPE)                         \
  do {                                                                    \
    for (size_t i = 0; i < MATRIX_ELEM_NUM(MAT_PTR); i++) {               \
      size_t tmp =                                                        \
          SEQUENCE_LIST_AT(MAT_ELEM(TYPE), (&((MAT_PTR)->data)), i)->i;   \
      SEQUENCE_LIST_AT(MAT_ELEM(TYPE), (&((MAT_PTR)->data)), i)->i =      \
          SEQUENCE_LIST_AT(MAT_ELEM(TYPE), (&((MAT_PTR)->data)), i)->j;   \
      SEQUENCE_LIST_AT(MAT_ELEM(TYPE), (&((MAT_PTR)->data)), i)->j = tmp; \
    }                                                                     \
    MATRIX_SET_SEQ(TYPE, (MAT_PTR), SEQ_TYPE);                            \
  } while (0)

#define MATRIX_ELEM_AT(TYPE, MAT_PTR, POS) \
  SEQUENCE_LIST_AT(MAT_ELEM(TYPE), (&((MAT_PTR)->data)), POS)

#define MATRIX_ELEM_REF(TYPE, MAT_PTR, POS) \
  SEQUENCE_LIST_REFERENCE(MAT_ELEM(TYPE), (&((MAT_PTR)->data)), POS)

#define MATRIX_CLEAR(TYPE, MAT_PTR) \
  SEQUENCE_LIST_RESIZE(MAT_ELEM(TYPE), (&((MAT_PTR)->data)), 0);

#define MATRIX_ADD(TYPE, DEST_PTR, SRC1_PTR, SRC2_PTR, SEQ)          \
  do {                                                               \
    MATRIX_SET_SEQ(TYPE, SRC1_PTR, ROW);                             \
    MATRIX_SET_SEQ(TYPE, SRC2_PTR, ROW);                             \
    MATRIX_CLEAR(TYPE, DEST_PTR);                                    \
    size_t pos1 = 0, pos2 = 0;                                       \
    while (pos1 < MATRIX_ELEM_NUM(SRC1_PTR) &&                       \
           pos2 < MATRIX_ELEM_NUM(SRC2_PTR)) {                       \
      if (MAT_ELEM_CMP(TYPE, row_first)(                             \
              MATRIX_ELEM_AT(TYPE, SRC1_PTR, pos1),                  \
              MATRIX_ELEM_AT(TYPE, SRC2_PTR, pos2)) > 0) {           \
        MAT_ELEM(TYPE) elem = MATRIX_ELEM_REF(TYPE, SRC2_PTR, pos2); \
        MATRIX_ADD_ELEM(TYPE, DEST_PTR, elem);                       \
        pos2++;                                                      \
      } else if (MAT_ELEM_CMP(TYPE, row_first)(                      \
                     MATRIX_ELEM_AT(TYPE, SRC1_PTR, pos1),           \
                     MATRIX_ELEM_AT(TYPE, SRC2_PTR, pos2)) < 0) {    \
        MAT_ELEM(TYPE) elem = MATRIX_ELEM_REF(TYPE, SRC1_PTR, pos1); \
        MATRIX_ADD_ELEM(TYPE, DEST_PTR, elem);                       \
        pos1++;                                                      \
      } else if (MAT_ELEM_CMP(TYPE, row_first)(                      \
                     MATRIX_ELEM_AT(TYPE, SRC1_PTR, pos1),           \
                     MATRIX_ELEM_AT(TYPE, SRC2_PTR, pos2)) == 0) {   \
        MAT_ELEM(TYPE) elem = MATRIX_ELEM_REF(TYPE, SRC1_PTR, pos1); \
        elem.elem += MATRIX_ELEM_REF(TYPE, SRC2_PTR, pos2).elem;     \
        MATRIX_ADD_ELEM(TYPE, DEST_PTR, elem);                       \
        pos1++;                                                      \
        pos2++;                                                      \
      }                                                              \
    }                                                                \
    while (pos1 < MATRIX_ELEM_NUM(SRC1_PTR)) {                       \
      MAT_ELEM(TYPE) elem = MATRIX_ELEM_REF(TYPE, SRC1_PTR, pos1);   \
      MATRIX_ADD_ELEM(TYPE, DEST_PTR, elem);                         \
      pos1++;                                                        \
    }                                                                \
    while (pos2 < MATRIX_ELEM_NUM(SRC2_PTR)) {                       \
      MAT_ELEM(TYPE) elem = MATRIX_ELEM_REF(TYPE, SRC2_PTR, pos2);   \
      MATRIX_ADD_ELEM(TYPE, DEST_PTR, elem);                         \
      pos2++;                                                        \
    }                                                                \
    MATRIX_SET_SEQ(TYPE, DEST_PTR, SEQ);                             \
  } while (0)

void destroy_matrix(matrix *mat) {
  destroy_sequence_list(&mat->data);
  mat->row_num = 0;
  mat->col_num = 0;
}

// Main program

DEFINE_MAT_ELEM_STRUCT(short)

int main(void) {
  size_t row_num, col_num, elem_num1, elem_num2;
  scanf("%ld%ld%ld%ld", &row_num, &col_num, &elem_num1, &elem_num2);

  matrix src1, src2, dest;
  INIT_MATRIX(short, &src1, row_num, col_num);
  INIT_MATRIX(short, &src2, row_num, col_num);
  INIT_MATRIX(short, &dest, row_num, col_num);

  for (size_t i = 0; i < elem_num1; i++) {
    MAT_ELEM(short) elem;
    scanf("%lu%lu%hd", &elem.i, &elem.j, &elem.elem);
    MATRIX_ADD_ELEM(short, &src1, elem);
  }

  for (size_t i = 0; i < elem_num2; i++) {
    MAT_ELEM(short) elem;
    scanf("%lu%lu%hd", &elem.i, &elem.j, &elem.elem);
    MATRIX_ADD_ELEM(short, &src2, elem);
  }

  MATRIX_ADD(short, &dest, &src1, &src2, ROW);

  for (size_t i = 0; i < MATRIX_ELEM_NUM(&dest); i++) {
    MAT_ELEM(short) elem = MATRIX_ELEM_REF(short, &dest, i);
    printf("%lu %lu %hu\n", elem.i, elem.j, elem.elem);
  }

  destroy_matrix(&src1);
  destroy_matrix(&src2);
  destroy_matrix(&dest);
}
