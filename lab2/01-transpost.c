#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 范型顺序表, 模拟C++的vectror.
// 范型顺序表维护了一整套的API, 用来实现自动扩容, 调整容量, 预分配内存等

// 计算较大元素的工具
#define MAX_OF(A, B) (((A) < (B)) ? (B) : (A))

// 顺序表数据结构定义.
// 为避免过多的结构体声明, 使用void *存储数据.
// 在调用多数API时, 需要手动传入存储数据的类型.
// 一个顺序表只建议存储一种数据, 乱写类型是未定义的!
typedef struct sequence_list {
  void *data;
  size_t size;
  size_t capacity;
} sequence_list;

// 再三思考后加入`列表大小`API, 在实现这个API之后结构体内部实现对用户透明,
// 降低软件偶合度, 用户不应该手动访问结构体内部成员
size_t sequence_list_size(sequence_list *lst) { return lst->size; }

// 初始化顺序表, 预分配大小为一个元素的内存单元
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

// 在顺序表末尾插入元素. 注意: 本次实现的顺序表只适用于POD, 即平凡数据类型.
// 也就是说, 需要拷贝构造函数,
// 移动构造函数和其它任何构造函数的结构体不应该用此表存储.
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

// 获得顺序表中特定位置元素的引用, C++中左值引用是相当重要的概念. 在C语言中,
// 引用的支持也能大幅度简化代码
#define SEQUENCE_LIST_REFERENCE(TYPE, LIST_PTR, POSITION) \
  (((TYPE *)((LIST_PTR)->data))[(POSITION) % ((LIST_PTR)->size)])

// 获得顺序表特定位置的指针
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

DEFINE_MAT_ELEM_STRUCT(short)

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

main() {
  matrix mat;

  size_t row_num, col_num;
  scanf("%ld%ld", &row_num, &col_num);
  INIT_MATRIX(short, &mat, row_num, col_num);

  while (1) {
    MAT_ELEM(short) elem;
    scanf("%ld%ld%hd", &elem.i, &elem.j, &elem.elem);
    if (elem.i == 0 && elem.j == 0 && elem.elem == 0) {
      break;
    }
    MATRIX_ADD_ELEM(short, &mat, elem);
  }
  MATRIX_TRANSPOSE(short, &mat, MASS);
  for (size_t i = 0; i < MATRIX_ELEM_NUM(&mat); i++) {
    MAT_ELEM(short) *curr = SEQUENCE_LIST_AT(MAT_ELEM(short), &mat.data, i);
    printf("%ld %ld %hd\n", curr->i, curr->j, curr->elem);
  }
}
