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

// 矩阵的十字链表表示法
// 本次十字链表照例使用侵入式链表实现. 但是考虑到在实际工程中,
// 十字链表的出场概率极低. 因此. 在定义数据类型时,
// 和上一次实验中的手工定义节点类型不同, 本次实验使用宏封装了结构体,
// 通过良好的封装, 可以对不同的数据类型自动声明不同的结构体,
// 然后对基本数据类型和POD类型实现便捷的数据操作.

// 由于宏定义函数中很难进行注释, 因此对于每个函数的讲解放在函数的头部

// 容器宏, 在Linux内核中得到广泛应用. 通过结构体成员指针,
// 结构体定义和成员定义自动计算外部结构体容器指针的地址. 本宏依赖offsetof标准宏.
// 在GNU C中, 这个宏通过未定义行为`((size_t)(&(((STRUCT
// *)(NULL))->member)))`实现.
#define CONTAINER_OF(NODE_TYPE, MEMBER, NODE_PTR) \
  ((NODE_TYPE *)((char *)(NODE_PTR) - (offsetof(NODE_TYPE, MEMBER))))

// 侵入式链表节点定义. 十字单向链表需要记录自己下方和右方的指针.
// 在十字链表中不应出现节点以外的其它元素.
// 但是考虑到十字链表在矩阵之外的应用几乎不存在, 因此在结构体中包含矩阵有关成员.
typedef struct intrusive_olnode {
  size_t i, j;  // Position of a node
  struct intrusive_olnode *right, *down;
} intrusive_olnode;

// 按照要求定义节点指针为olink.
// 在此处olink的定义可以在简化`sequence_list`的操作书写难度(类型变成olink而非intrusive_olnode
// *, 代码规范, 工整, 清晰, 美观).
typedef intrusive_olnode *olink;

// 矩阵结构体定义. 首先存在两个头节点数组, 其长度分别为行数和列数.
// 十字链表矩阵同时需要存储行数, 列数和元素个数.
// 本次的实现中将行头节点和列头节点分离而非合并, 更加符合人的阅读习惯,
// 代码逻辑更加清晰.
typedef struct olmatrix {
  // Sequence list of type olink
  sequence_list row_head;
  sequence_list col_head;
  size_t row_num, col_num;
  size_t elem_num;
} olmatrix;

// 声明节点的结构体
#define GEN_OLNODE_STRUCT(TYPE)  \
  typedef struct olnode_##TYPE { \
    TYPE elem;                   \
    intrusive_olnode node;       \
  } olnode_##TYPE;

// 节点结构体类型
#define OLNODE(TYPE) olnode_##TYPE

// olmatrix构造函数, 为头节点分配内存, 并且进行初始化, 将其指针指向NULL
// 行和列下标都从1开始, 符合数学习惯
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

// 为矩阵添加元素. 处于使用的遍历性考虑, 本次添加元素不通过传统的MAKE_NODE,
// CONTAINER_OF 赋值和 INSERT_BEHIND/INSERT_IN_FRONT_OF步骤,
// 而是直接在函数内构造节点并且插入. 在构造节点之后,
// 先在对应行从左到右进行顺序查找并插入, 接着在相应列进行从上到下查找并插入.
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

// 获取行迭代器(头节点指针)
olink olmatrix_row_iterator(olmatrix *mat, size_t i) {
  return SEQUENCE_LIST_REFERENCE(olink, (&(mat->row_head)), i);
}

// 获取列迭代器
olink olmatrix_col_iterator(olmatrix *mat, size_t i) {
  return SEQUENCE_LIST_REFERENCE(olink, (&(mat->col_head)), i);
}

// 业务代码, 两个矩阵相加.
// 诸行相加, 从左到右
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
          if (elem_val) {                                                 \
            OLMATRIX_ADD_ELEM(TYPE, DEST, i, it1->j, elem_val);           \
          }                                                               \
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
