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

// Matrix of cross linked list.

#define CONTAINER_OF(NODE_TYPE, MEMBER, NODE_PTR) \
  ((NODE_TYPE *)((char *)(NODE_PTR) - (offsetof(NODE_TYPE, MEMBER))))

typedef struct intrusive_olnode {
  size_t i, j;  // Position of a node
  struct intrusive_olnode *right, *down;
} intrusive_olnode;

typedef intrusive_olnode *olink;

typedef struct olmatrix {
  // Sequence list of type olink
  sequence_list row_head;
  sequence_list col_head;
  size_t row_num, col_num;
  size_t elem_num;
} olmatrix;

#define GEN_OLNODE_STRUCT(TYPE)  \
  typedef struct olnode_##TYPE { \
    TYPE elem;                   \
    intrusive_olnode node;       \
  } olnode_##TYPE;

#define OLNODE(TYPE) olnode_##TYPE

int init_olmatrix(olmatrix *mat, size_t row_num, size_t col_num) {
  INIT_SEQUENCE_LIST(olink, (&((mat)->row_head)));
  SEQUENCE_LIST_RESIZE(olink, (&((mat)->row_head)), ((row_num) + 1));
  INIT_SEQUENCE_LIST(olink, (&((mat)->col_head)));
  SEQUENCE_LIST_RESIZE(olink, (&((mat)->col_head)), ((col_num) + 1));
  SEQUENCE_LIST_REFERENCE(olink, (&(mat->row_head)), 0) = NULL;
  SEQUENCE_LIST_REFERENCE(olink, (&(mat->col_head)), 0) = NULL;

  for (size_t i = row_num; i >= 1; i--) {
    SEQUENCE_LIST_REFERENCE(olink, (&(mat->row_head)), i) =
        (olink)(malloc(sizeof(intrusive_olnode)));
    if (SEQUENCE_LIST_REFERENCE(olink, (&(mat->row_head)), i) == NULL) {
      perror("Fail to init the matrix");
      for (size_t j = row_num; j >= i; j--) {
        free(SEQUENCE_LIST_REFERENCE(olink, (&(mat->row_head)), j));
      }
      destroy_sequence_list((&(mat->row_head)));
      destroy_sequence_list((&(mat->col_head)));
      return -1;
    }
    SEQUENCE_LIST_REFERENCE(olink, (&(mat->row_head)), i)->i = i;
    SEQUENCE_LIST_REFERENCE(olink, (&(mat->row_head)), i)->j = 0;
    SEQUENCE_LIST_REFERENCE(olink, (&(mat->row_head)), i)->down = NULL;
    SEQUENCE_LIST_REFERENCE(olink, (&(mat->row_head)), i)->right = NULL;
    if (i != row_num) {
      SEQUENCE_LIST_REFERENCE(olink, (&(mat->row_head)), i)->down =
          SEQUENCE_LIST_REFERENCE(olink, (&(mat->row_head)), i + 1);
    }
  }

  for (size_t i = col_num; i >= 1; i--) {
    SEQUENCE_LIST_REFERENCE(olink, (&(mat->col_head)), i) =
        (olink)(malloc(sizeof(intrusive_olnode)));
    if (SEQUENCE_LIST_REFERENCE(olink, (&(mat->col_head)), i) == NULL) {
      perror("Fail to init the matrix");
      for (size_t j = col_num; j >= i; j--) {
        free(SEQUENCE_LIST_REFERENCE(olink, (&(mat->col_head)), j));
      }
      destroy_sequence_list((&(mat->row_head)));
      destroy_sequence_list((&(mat->col_head)));
      return -1;
    }
    SEQUENCE_LIST_REFERENCE(olink, (&(mat->col_head)), i)->i = 0;
    SEQUENCE_LIST_REFERENCE(olink, (&(mat->col_head)), i)->j = i;
    SEQUENCE_LIST_REFERENCE(olink, (&(mat->col_head)), i)->down = NULL;
    SEQUENCE_LIST_REFERENCE(olink, (&(mat->col_head)), i)->right = NULL;
    if (i != col_num) {
      SEQUENCE_LIST_REFERENCE(olink, (&(mat->col_head)), i)->right =
          SEQUENCE_LIST_REFERENCE(olink, (&(mat->col_head)), i + 1);
    }
  }
  
  mat->row_num = row_num;
  mat->col_num = col_num;
  
  return 0;
}

#define OLMATRIX_ADD_ELEM(TYPE, MAT_PTR, ROW, COL, ELEM)                      \
  do {                                                                        \
    OLNODE(TYPE) *node = (OLNODE(TYPE) *)(malloc(sizeof(OLNODE(TYPE))));      \
    olink target = (&((node)->node));                                         \
    target->i = (ROW);                                                        \
    target->j = (COL);                                                        \
    node->elem = (ELEM);                                                      \
    for (olink it = (SEQUENCE_LIST_REFERENCE(olink, (&((MAT_PTR)->row_head)), \
                                             (ROW)));                         \
         it != NULL; it = it->right) {                                        \
      if (it->right == NULL || it->right->j > target->j) {                    \
        target->right = it->right;                                            \
        it->right = target;                                                   \
        break;                                                                \
      }                                                                       \
    }                                                                         \
    for (olink it = (SEQUENCE_LIST_REFERENCE(olink, (&((MAT_PTR)->col_head)), \
                                             (COL)));                         \
         it != NULL; it = it->down) {                                         \
      if (it->down == NULL || it->down->i > target->i) {                      \
        target->down = it->down;                                              \
        it->down = target;                                                    \
        break;                                                                \
      }                                                                       \
    }                                                                         \
    (MAT_PTR)->elem_num++;                                                    \
  } while (0)

olink olmatrix_row_iterator(olmatrix *mat, size_t i) {
  return SEQUENCE_LIST_REFERENCE(olink, (&(mat->row_head)), i);
}

olink olmatrix_col_iterator(olmatrix *mat, size_t i) {
  return SEQUENCE_LIST_REFERENCE(olink, (&(mat->col_head)), i);
}

#define OLMATRIX_ADD(TYPE, DEST, SRC1, SRC2)                              \
  do {                                                                    \
    for (size_t i = 1; i <= row_num; i++) {                               \
      olink it1 = olmatrix_row_iterator((SRC1), i)->right,                \
            it2 = olmatrix_row_iterator((SRC2), i)->right;                \
      while (it1 != NULL && it2 != NULL) {                                \
        if (it1->j > it2->j) {                                            \
          OLMATRIX_ADD_ELEM(TYPE, DEST, i, it2->j,                        \
                            CONTAINER_OF(OLNODE(TYPE), node, it2)->elem); \
          it2 = it2->right;                                               \
        } else if (it1->j < it2->j) {                                     \
          OLMATRIX_ADD_ELEM(TYPE, DEST, i, it1->j,                        \
                            CONTAINER_OF(OLNODE(TYPE), node, it1)->elem); \
          it1 = it1->right;                                               \
        } else {                                                          \
          TYPE elem_val = CONTAINER_OF(OLNODE(TYPE), node, it1)->elem +   \
                          CONTAINER_OF(OLNODE(TYPE), node, it2)->elem;    \
          OLMATRIX_ADD_ELEM(TYPE, DEST, i, it1->j, elem_val);             \
          it1 = it1->right;                                               \
          it2 = it2->right;                                               \
        }                                                                 \
      }                                                                   \
      while (it1 != NULL) {                                               \
        OLMATRIX_ADD_ELEM(TYPE, DEST, i, it1->j,                          \
                          CONTAINER_OF(OLNODE(TYPE), node, it1)->elem);   \
        it1 = it1->right;                                                 \
      }                                                                   \
      while (it2 != NULL) {                                               \
        OLMATRIX_ADD_ELEM(TYPE, DEST, i, it2->j,                          \
                          CONTAINER_OF(OLNODE(TYPE), node, it2)->elem);   \
        it2 = it2->right;                                                 \
      }                                                                   \
    }                                                                     \
  } while (0)

#define DESTROY_OLMATRIX(TYPE, MAT_PTR)                                  \
  do {                                                                   \
    for (size_t i = 1; i <= (MAT_PTR)->row_num; i++) {                   \
      olink it = olmatrix_row_iterator((MAT_PTR), i)->right;             \
      while (it != NULL) {                                               \
        olink old_it = it;                                               \
        it = it->right;                                                  \
        OLNODE(TYPE) *target = CONTAINER_OF(OLNODE(TYPE), node, old_it); \
        free(target);                                                    \
      }                                                                  \
    }                                                                    \
    for (size_t i = 1; i <= (MAT_PTR)->row_num; i++) {                   \
      free(SEQUENCE_LIST_REFERENCE(olink, (&((MAT_PTR)->row_head)), i)); \
    }                                                                    \
    for (size_t i = 1; i <= (MAT_PTR)->col_num; i++) {                   \
      free(SEQUENCE_LIST_REFERENCE(olink, (&((MAT_PTR)->col_head)), i)); \
    }                                                                    \
    destroy_sequence_list((&(MAT_PTR)->row_head));                       \
    destroy_sequence_list((&(MAT_PTR)->col_head));                       \
  } while (0)

int main() {
  setbuf(stdout, NULL);
  GEN_OLNODE_STRUCT(short);
  olmatrix dest, src1, src2;
  size_t row_num, col_num, elem_num1, elem_num2;
  scanf("%lu%lu%lu%lu", &row_num, &col_num, &elem_num1, &elem_num2);
  init_olmatrix(&dest, row_num, col_num);
  init_olmatrix(&src1, row_num, col_num);
  init_olmatrix(&src2, row_num, col_num);

  for (size_t i = 0; i < elem_num1; i++) {
    size_t i, j;
    short elem_val;
    scanf("%lu%lu%hd", &i, &j, &elem_val);
    OLMATRIX_ADD_ELEM(short, &src1, i, j, elem_val);
  }
  for (size_t i = 0; i < elem_num2; i++) {
    size_t i, j;
    short elem_val;
    scanf("%lu%lu%hd", &i, &j, &elem_val);
    OLMATRIX_ADD_ELEM(short, &src2, i, j, elem_val);
  }

  OLMATRIX_ADD(short, &dest, &src1, &src2);

  for (size_t i = 1; i <= row_num; i++) {
    olink it = olmatrix_row_iterator(&dest, i)->right;
    while (it != NULL) {
      OLNODE(short) *dat = CONTAINER_OF(OLNODE(short), node, it);
      printf("%lu %lu %hd\n", it->i, it->j, dat->elem);
      it = it->right;
    }
  }

  DESTROY_OLMATRIX(short, &src1);
  DESTROY_OLMATRIX(short, &src2);
  DESTROY_OLMATRIX(short, &dest);
}
