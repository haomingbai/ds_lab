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

#define SEQUENCE_LIST_POP_BACK(TYPE, LIST_PTR, VAL)                 \
  do {                                                              \
    ((LIST_PTR)->size)--;                                           \
    if (((LIST_PTR)->capacity) > (((LIST_PTR)->size) * 4 + 1)) {    \
      size_t new_capacity = MAX_OF(((LIST_PTR)->size), 1);          \
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
      size_t new_capacity = MAX_OF(((LIST_PTR)->size), 1);          \
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

// Specific data structure.

typedef struct sequence_queue {
  size_t begin_pos;
  size_t end_pos;
  sequence_list lst;
} sequence_queue;

#define INIT_SEQUENCE_QUEUE(TYPE, Q_PTR, CAPACITY)           \
  do {                                                       \
    INIT_SEQUENCE_LIST(TYPE, (&((Q_PTR)->lst)));             \
    SEQUENCE_LIST_RESIZE(TYPE, (&((Q_PTR)->lst)), CAPACITY); \
    ((Q_PTR)->begin_pos) = 0;                                \
    ((Q_PTR)->end_pos) = 0;                                  \
  } while (0)

bool sequence_queue_empty(sequence_queue *queue) {
  return queue->begin_pos == queue->end_pos;
}

bool sequence_queue_full(sequence_queue *queue) {
  return (queue->end_pos - queue->begin_pos) >= queue->lst.size;
}

#define SEQUENCE_QUEUE_PUSH(TYPE, Q_PTR, VAL)                                  \
  do {                                                                         \
    if (!sequence_queue_full((Q_PTR))) {                                       \
      SEQUENCE_LIST_REFERENCE(TYPE, (&((Q_PTR)->lst)), (((Q_PTR)->end_pos))) = \
          (VAL);                                                               \
      ((Q_PTR)->end_pos)++;                                                    \
    } else {                                                                   \
      perror("The queue is full, giving up...");                               \
    }                                                                          \
  } while (0)

void sequence_queue_pop(sequence_queue *queue) {
  if (!sequence_queue_empty(queue)) {
    (queue->begin_pos)++;
    if (queue->begin_pos >= queue->lst.size) {
      queue->begin_pos -= queue->lst.size;
      queue->end_pos -= queue->lst.size;
    }
  } else {
    perror("The queue is already empty, giving up...");
  }
}
#define SEQUENCE_QUEUE_FRONT(TYPE, Q_PTR) \
  SEQUENCE_LIST_REFERENCE(TYPE, (&((Q_PTR)->lst)), ((Q_PTR)->begin_pos))

void destroy_sequence_queue(sequence_queue *queue) {
  destroy_sequence_list(&queue->lst);
  queue->begin_pos = 0;
  queue->end_pos = 0;
}

int main() {
  sequence_queue queue;
  INIT_SEQUENCE_QUEUE(char, &queue, 5);

  SEQUENCE_QUEUE_PUSH(char, &queue, 'a');
  SEQUENCE_QUEUE_PUSH(char, &queue, 'b');
  SEQUENCE_QUEUE_PUSH(char, &queue, 'c');
  SEQUENCE_QUEUE_PUSH(char, &queue, 'd');
  SEQUENCE_QUEUE_PUSH(char, &queue, 'e');

  // These operations will fail for it will fail.
  SEQUENCE_QUEUE_PUSH(char, &queue, 'f');
  SEQUENCE_QUEUE_PUSH(char, &queue, 'g');

  while (!sequence_queue_empty(&queue)) {
    char x = SEQUENCE_QUEUE_FRONT(char, &queue);
    sequence_queue_pop(&queue);
    putchar(x);
  }

  putchar('\n');

  SEQUENCE_QUEUE_PUSH(char, &queue, 'a');
  SEQUENCE_QUEUE_PUSH(char, &queue, 'b');
  SEQUENCE_QUEUE_PUSH(char, &queue, 'c');

  for (size_t i = 'd'; i <= 'z'; i++) {
    char x = SEQUENCE_QUEUE_FRONT(char, &queue);
    sequence_queue_pop(&queue);
    SEQUENCE_QUEUE_PUSH(char, &queue, i);
    putchar(x);
  }
  putchar('\t');
  putchar(SEQUENCE_QUEUE_FRONT(char, &queue));
  sequence_queue_pop(&queue);
  putchar(SEQUENCE_QUEUE_FRONT(char, &queue));
  sequence_queue_pop(&queue);
  putchar(SEQUENCE_QUEUE_FRONT(char, &queue));
  sequence_queue_pop(&queue);
  putchar('\n');

  destroy_sequence_queue(&queue);
}
