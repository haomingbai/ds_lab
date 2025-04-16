#include "sequence_list.c"

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

#define MATRIX_ELEM_NUM(MAT_PTR) ((size_t)(((MAT_PTR)->data).size))

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

#define MATRIX_MULTIPLICATION(TYPE, DEST_PTR, SRC1_PTR, SRC2_PTR, SEQ)        \
  do {                                                                        \
    MATRIX_SET_SEQ(TYPE, SRC1_PTR, ROW);                                      \
    MATRIX_SET_SEQ(TYPE, SRC2_PTR, COL);                                      \
    MATRIX_CLEAR(TYPE, DEST_PTR);                                             \
    for (size_t pos1 = 0; pos1 < MATRIX_ELEM_NUM(SRC1_PTR);) {                \
      size_t row_pos = MATRIX_ELEM_REF(TYPE, SRC1_PTR, pos1).i;               \
      size_t this_row_len = 0;                                                \
      while (pos1 + this_row_len < MATRIX_ELEM_NUM(SRC1_PTR) &&               \
             MATRIX_ELEM_REF(TYPE, SRC1_PTR, pos1 + this_row_len).i ==        \
                 row_pos) {                                                   \
        this_row_len++;                                                       \
      }                                                                       \
      for (size_t pos2 = 0; pos2 < MATRIX_ELEM_NUM(SRC2_PTR);) {              \
        size_t col_pos = MATRIX_ELEM_REF(TYPE, SRC2_PTR, pos2).j;             \
        MAT_ELEM(TYPE) new_elem;                                              \
        new_elem.j = col_pos;                                                 \
        new_elem.i = row_pos;                                                 \
        new_elem.elem = 0;                                                    \
        size_t tmp_pos1 = pos1;                                               \
        while (pos2 < MATRIX_ELEM_NUM(SRC2_PTR) &&                            \
               MATRIX_ELEM_REF(TYPE, SRC2_PTR, pos2).j == col_pos &&          \
               tmp_pos1 < pos1 + this_row_len) {                              \
          if (MATRIX_ELEM_REF(TYPE, SRC1_PTR, tmp_pos1).j ==                  \
              MATRIX_ELEM_REF(TYPE, SRC2_PTR, pos2).i) {                      \
            new_elem.elem += MATRIX_ELEM_REF(TYPE, SRC1_PTR, tmp_pos1).elem * \
                             MATRIX_ELEM_REF(TYPE, SRC2_PTR, pos2).elem;      \
            tmp_pos1++;                                                       \
            pos2++;                                                           \
          } else if (MATRIX_ELEM_REF(TYPE, SRC1_PTR, tmp_pos1).j <            \
                     MATRIX_ELEM_REF(TYPE, SRC2_PTR, pos2).i) {               \
            tmp_pos1++;                                                       \
          } else if (MATRIX_ELEM_REF(TYPE, SRC1_PTR, tmp_pos1).j >            \
                     MATRIX_ELEM_REF(TYPE, SRC2_PTR, pos2).i) {               \
            pos2++;                                                           \
          }                                                                   \
        }                                                                     \
        while (pos2 < MATRIX_ELEM_NUM(SRC2_PTR) &&                            \
               MATRIX_ELEM_REF(TYPE, SRC2_PTR, pos2).j == col_pos) {          \
          pos2++;                                                             \
        }                                                                     \
        if (new_elem.elem != 0) {                                             \
          MATRIX_ADD_ELEM(TYPE, DEST_PTR, new_elem);                          \
        }                                                                     \
      }                                                                       \
      pos1 += this_row_len;                                                   \
    }                                                                         \
  } while (0)

void destroy_matrix(matrix *mat) {
  destroy_sequence_list(&mat->data);
  mat->row_num = 0;
  mat->col_num = 0;
}
