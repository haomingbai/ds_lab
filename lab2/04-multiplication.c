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

// 删除顺序表最后一个位置的元素. 因为这个操作和push_back同样常用.
// 所以本次程序实现和C++中的std::vector相同, 单独实现了push_back和pop_back
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

// 在顺序表特定位置插入元素. 注意, 插入操作的复杂度为`O(n)`, 慎用!
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

// 删除特定位置的元素, 慎用!
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

// 预分配内存, 在列表长度改变频繁, 但是最大可能容量固定的条件下会使用
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

// 顺序表尺寸改变, 尺寸改变之后考虑到时间复杂度, 未对新元素进行初始化
// 注意: 在尺寸改编后可能导致数据丢失!
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

// 内存收缩. 默认条件下当顺序表的尺寸变为容量的1/8时才进行缩容,
// 但是这个过程可以手动进行, 以节省内存.
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

// 顺序表析构函数
void destroy_sequence_list(sequence_list *lst) {
  free(lst->data);
  lst->capacity = 0;
  lst->size = 0;
}

// 判断顺序表是否为空表, 这个函数非常常用, 在C++的vector中有实现,
// 因此本次代码编写中进行了实现
bool sequence_list_empty(sequence_list *lst) { return lst->size == 0; }

// 三元组的矩阵存储

// 由于宏定义函数中很难进行注释, 因此对于每个函数的讲解放在函数的头部

// 声明一个类型的矩阵元素, 调用时传入类型, 然后自动生成结构体和相应的比较函数
// 这一设计主要是为了配合C语言标准库中的`qsort`函数
// 在声明结构体的同时声明了三种比较函数: 值为主的比较函数,
// 行为主的比较函数和列为主的比较函数.
// 注意: 本函数及之后所有函数的类型要求为POD且只有一个单词,
// 如果有多个单词可以使用`typedef`定义为一个单词
// 例如: typedef unsigned long long ull;
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

// 传入数据类型生成元素类型
#define MAT_ELEM(TYPE) mat_elem_##TYPE

// 传入数据类型和比较函数类型传入比较函数类型返回比较函数
#define MAT_ELEM_CMP(TYPE, CMP_TYPE) mat_elem_##TYPE##_##CMP_TYPE##_cmp

// 矩阵中元素排序的类型
enum seq_type { ROW, COL, VAL, MASS };

typedef struct matrix {
  // The data of elements.
  sequence_list data;
  size_t row_num, col_num;
} matrix;

// 初始化三元组矩阵列表.
#define INIT_MATRIX(TYPE, MAT_PTR, ROW_NUM, COL_NUM)          \
  do {                                                        \
    INIT_SEQUENCE_LIST(MAT_ELEM(TYPE), (&((MAT_PTR)->data))); \
    ((MAT_PTR)->row_num) = (ROW_NUM);                         \
    ((MAT_PTR)->col_num) = (COL_NUM);                         \
  } while (0)

// 为一个矩阵添加一个元素. 注意: 三思而后行, 没有后悔药! 没有相应的删除函数,
// 因为删除的API很难定义...
#define MATRIX_ADD_ELEM(TYPE, MAT_PTR, ELEM)                               \
  do {                                                                     \
    SEQUENCE_LIST_PUSH_BACK(MAT_ELEM(TYPE), (&((MAT_PTR)->data)), (ELEM)); \
  } while (0)

// 返回矩阵元素个数, 我也不知道这里当时为什么要用宏实现...
#define MATRIX_ELEM_NUM(MAT_PTR) (sequence_list_size((&((MAT_PTR)->data))))

// 为矩阵元素排序, 这样可以适应不同种类的运算. 例如在加法中,
// 需要采用行主元顺序或者列主元顺序,
// 在乘法中需要将A矩阵置为行主元同时将B置为列主元
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

// 业务代码, 矩阵就地转置.
// 将矩阵的每一个元素的i和j交换即可
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

// 获得特定位置矩阵元素的指针. 注意, 这里的"特定位置"不是在矩阵中的位置,
// 而是在数组中的位置, 因为多行宏定义函数不支持多行函数返回,
// 但是单行函数无法表达特定位置查找语义.
#define MATRIX_ELEM_AT(TYPE, MAT_PTR, POS) \
  SEQUENCE_LIST_AT(MAT_ELEM(TYPE), (&((MAT_PTR)->data)), POS)

// 获得特定位置的矩阵元素引用.
#define MATRIX_ELEM_REF(TYPE, MAT_PTR, POS) \
  SEQUENCE_LIST_REFERENCE(MAT_ELEM(TYPE), (&((MAT_PTR)->data)), POS)

// 清空矩阵中的所有元素,
// 可以确保加法和乘法中目标矩阵的正确性(在计算开始前需要确保目标矩阵是空矩阵,
// 但是目标矩阵, 出于软件工程规范性, 不得要求传入未构造的矩阵)
#define MATRIX_CLEAR(TYPE, MAT_PTR) \
  SEQUENCE_LIST_RESIZE(MAT_ELEM(TYPE), (&((MAT_PTR)->data)), 0);

// 业务代码: 矩阵加法
// 这段程序需要首先确保两个矩阵处于行主元状态, 然后设置两个迭代器,
// 分别指向正在处理的矩阵元素. 因为在行主元条件下, 两个大小相等的矩阵,
// 其中任意两个元素的位置有一个唯一确定的映射, 且在正常的矩阵中,
// 这个映射应当是良序的. 而当其中一个迭代器在向后迭代时,
// 那么其位置在这一映射中一定向更大的方向发展. 因此, 我们可以得到三种情况:
// itA > itB, itA == itB, itA < itB, 在这一条件下,
// 将较小的迭代器在插入相应元素进入新矩阵后向后迭代,
// 在两迭代器相等时对于两个迭代器对应的元素进行相加再同时进行迭代,
// 正确性是可以得到保证的.
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

// 业务代码, 矩阵乘法.
// 在清空目标矩阵之后, 对于前方矩阵进行行主元排序, 对于后方矩阵进行列主元排序.
// 然后将非0的每一行和相对应的列分别进行迭代相乘.
// 当且仅当存在A矩阵的第i行j列和B矩阵的j行k列元素不为0时,
// 目的地的i行k列的矩阵可能不为0. 在完成乘法之后, 如果乘法之后的结果不为0,
// 则为目标矩阵添加元素.
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

// 三元组矩阵的析构函数
void destroy_matrix(matrix *mat) {
  destroy_sequence_list(&mat->data);
  mat->row_num = 0;
  mat->col_num = 0;
}

// Main program

DEFINE_MAT_ELEM_STRUCT(short)

int main(void) {
  matrix src1, src2, dest;

  size_t row1_num, col1_num;
  scanf("%ld%ld", &row1_num, &col1_num);
  INIT_MATRIX(short, &src1, row1_num, col1_num);
  while (1) {
    MAT_ELEM(short) elem;
    scanf("%lu%lu%hu", &elem.i, &elem.j, &elem.elem);
    if (elem.i == 0 && elem.j == 0 && elem.elem == 0) {
      break;
    }
    MATRIX_ADD_ELEM(short, &src1, elem);
  }

  size_t row2_num, col2_num;
  scanf("%lu%lu", &row2_num, &col2_num);
  INIT_MATRIX(short, &src2, row2_num, col2_num);
  while (1) {
    MAT_ELEM(short) elem;
    scanf("%lu%lu%hu", &elem.i, &elem.j, &elem.elem);
    if (elem.i == 0 && elem.j == 0 && elem.elem == 0) {
      break;
    }
    MATRIX_ADD_ELEM(short, &src2, elem);
  }

  INIT_MATRIX(short, &dest, row1_num, col2_num);

  MATRIX_MULTIPLICATION(short, &dest, &src1, &src2, ROW);

  for (size_t i = 0; i < MATRIX_ELEM_NUM(&dest); i++) {
    MAT_ELEM(short) elem = MATRIX_ELEM_REF(short, &dest, i);
    printf("%lu %lu %hd\n", elem.i, elem.j, elem.elem);
  }

  destroy_matrix(&src1);
  destroy_matrix(&src2);
  destroy_matrix(&dest);
}
