#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

// Stack ...

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

// List ...

/**
 * @brief Get the pointer to the containing structure from a member pointer.
 *
 * This macro is commonly used in intrusive data structures to retrieve the
 * address of the parent structure given a pointer to one of its members. It is
 * similar to the Linux kernel's `container_of` macro.
 *
 * @param NODE_TYPE The type of the containing structure.
 * @param MEMBER    The name of the member within the structure.
 * @param NODE_PTR  Pointer to the member within the structure.
 *
 * @return Pointer to the containing structure of type `NODE_TYPE*`.
 *
 * @note The behavior is undefined if:
 *       - `NODE_PTR` does not point to a valid member of a `NODE_TYPE`
 * structure
 *       - `MEMBER` is not a valid member of `NODE_TYPE`
 *
 * @warning This macro performs pointer arithmetic. Ensure proper alignment and
 *          valid pointer usage to avoid undefined behavior.
 *
 * Example usage:
 * @code
 * struct parent {
 *     int value;
 *     IntrusiveNode node;
 * };
 *
 * struct list_node *child_ptr = ...;
 * struct parent *parent_ptr = CONTAINER_OF(struct parent, node, child_ptr);
 * @endcode
 */
#define CONTAINER_OF(NODE_TYPE, MEMBER, NODE_PTR) \
  ((NODE_TYPE *)((char *)(NODE_PTR) - (offsetof(NODE_TYPE, MEMBER))))

/**
 * @brief Dynamically allocates a node and returns its intrusive node member.
 *
 * This macro allocates memory for a structure of type `NODE_TYPE`, then returns
 * a pointer to its `MEMBER` field (which should be of type `IntrusiveNode`).
 * It is useful for initializing intrusive data structures where the node is
 * embedded within a larger structure.
 *
 * @param NODE_TYPE The type of the containing structure to allocate.
 * @param MEMBER    The name of the intrusive node member within the structure.
 *
 * @return Pointer to the `IntrusiveNode` member of the newly allocated
 * structure.
 *
 * @note The caller is responsible for freeing the allocated memory.
 * @warning This macro performs memory allocation. Check for NULL return values
 *          in real usage (the returned pointer will be NULL if malloc fails).
 * @warning The `MEMBER` must be properly aligned within `NODE_TYPE` to avoid
 *          undefined behavior.
 *
 * Example usage:
 * @code
 * struct my_struct {
 *     int data;
 *     IntrusiveNode node;
 * };
 *
 * IntrusiveNode *new_node = MAKE_NODE(struct my_struct, node);
 * if (!new_node) {
 *     // Handle allocation failure
 * }
 * @endcode
 */
#define MAKE_NODE(NODE_TYPE, MEMBER) \
  ((IntrusiveNode *)(&(((NODE_TYPE *)(malloc(sizeof(NODE_TYPE))))->MEMBER)))

/**
 * @brief Removes a node from an intrusive linked list.
 *
 * This macro removes the specified node from the list while maintaining list
 * integrity. It decrements the list size counter and returns the removed node's
 * IntrusiveNode pointer. The actual removal is performed by the internal
 * function _remove_Node_WithoutChangingSize().
 *
 * @param LIST_PTR Pointer to the intrusive list structure (must have 'size'
 * member).
 * @param NODE_PTR Pointer to the intrusive node to be removed from the list.
 *
 * @return Pointer to the removed IntrusiveNode (same as NODE_PTR).
 *
 * @note The list's size counter will be decremented by 1.
 * @warning The LIST_PTR must point to a valid list structure with a 'size'
 * member.
 * @warning The NODE_PTR must be a valid node currently in the list.
 * @warning This macro has side effects - it modifies the list structure.
 *
 * Example usage:
 * @code
 * IntrusiveList my_list;
 * IntrusiveNode* node_to_remove = ...;
 *
 * IntrusiveNode* removed = REMOVE_NODE_FROM_LIST(&my_list, node_to_remove);
 * // removed == node_to_remove
 * // my_list.size has been decremented
 * @endcode
 */
#define REMOVE_NODE_FROM_LIST(LIST_PTR, NODE_PTR) \
  (((LIST_PTR)->size--), _remove_Node_WithoutChangingSize(NODE_PTR));

/**
 * @brief Removes a node from a list and releases its memory.
 *
 * This macro performs two operations atomically:
 * 1. Removes the specified node from the intrusive list
 * 2. Frees the memory of the containing structure
 *
 * @param NODE_TYPE The type of the structure containing the intrusive node.
 * @param MEMBER    The name of the intrusive node member within the structure.
 * @param LIST_PTR  Pointer to the intrusive list structure.
 * @param NODE_PTR  Pointer to the intrusive node to be removed and freed.
 *
 * @note After this operation:
 *       - The node is no longer part of the list
 *       - The containing structure's memory is freed
 *       - Any pointers to the structure become invalid
 *
 * @warning This macro will FREE MEMORY - do not use the node pointer
 * afterwards.
 * @warning The LIST_PTR must point to a valid list structure.
 * @warning The NODE_PTR must be a valid node currently in the list.
 *
 * Example usage:
 * @code
 * struct item {
 *     int value;
 *     IntrusiveNode node;
 * };
 *
 * IntrusiveList list;
 * IntrusiveNode* node = ...;  // Get node from somewhere
 *
 * // Node is removed and its memory freed
 * REMOVE_AND_RELEASE(struct item, node, &list, node);
 * // 'node' pointer is now INVALID
 * @endcode
 */
#define REMOVE_AND_RELEASE(NODE_TYPE, MEMBER, LIST_PTR, NODE_PTR) \
  do {                                                            \
    IntrusiveNode *_NODE_TO_DEL = NODE_PTR;                       \
    assert(_NODE_TO_DEL != (LIST_PTR)->head);                     \
    REMOVE_NODE_FROM_LIST(LIST_PTR, _NODE_TO_DEL);                \
    free(CONTAINER_OF(NODE_TYPE, MEMBER, _NODE_TO_DEL));          \
  } while (0)

/**
 * @brief Checks if an intrusive list is empty.
 *
 * This macro evaluates whether the specified intrusive list contains any
 * elements by checking its size counter.
 *
 * @param LIST_PTR Pointer to the intrusive list structure (must have 'size'
 * member).
 *
 * @return true if the list is empty (size == 0), false otherwise.
 *
 * @note This is a constant-time operation (O(1)).
 * @warning The LIST_PTR must point to a valid list structure with a 'size'
 * member.
 *
 * Example usage:
 * @code
 * IntrusiveList my_list;
 *
 * if (IS_EMPTY(&my_list)) {
 *     printf("The list is empty\n");
 * } else {
 *     printf("The list contains %zu elements\n", my_list.size);
 * }
 * @endcode
 */
#define IS_EMPTY(LIST_PTR) (!((LIST_PTR)->size))

/**
 * @brief Destroys an entire intrusive list and releases all node memory.
 *
 * This macro safely removes and frees all nodes in the list, effectively
 * destroying the list contents while keeping the list structure itself valid
 * (but empty).
 *
 * @param NODE_TYPE The type of the structure containing intrusive nodes.
 * @param MEMBER    The name of the intrusive node member within the structure.
 * @param LIST_TYPE The type of the list structure.
 * @param LIST_PTR  Pointer to the list to be destroyed.
 *
 * @warning This will FREE ALL MEMORY used by nodes in the list.
 * @warning After this operation:
 *          - All node pointers from this list become INVALID
 *          - The list will be empty (size == 0)
 *          - The list structure itself remains valid
 * @warning The LIST_PTR must point to a valid initialized list.
 * @warning The list to be destroyed should have
 * _freeEmpty_##LIST_TYPE##_(LIST_TYPE *);
 *
 * @note For thread safety, ensure no other threads are accessing the list
 * during destruction.
 *
 * Example usage:
 * @code
 * struct item {
 *     int data;
 *     IntrusiveNode node;
 * };
 *
 * IntrusiveList my_list;
 * // ... populate list ...
 *
 * DESTROY_LIST(struct item, node, IntrusiveList, &my_list);
 * // my_list is now empty and all item memory is freed
 * @endcode
 */
#define DESTROY_LIST(NODE_TYPE, MEMBER, LIST_TYPE, LIST_PTR)      \
  do {                                                            \
    while ((LIST_PTR)->size) {                                    \
      assert(((LIST_PTR)->head)->next->prev == (LIST_PTR)->head); \
      REMOVE_AND_RELEASE(NODE_TYPE, MEMBER, LIST_PTR,             \
                         (((LIST_PTR)->head)->next));             \
    }                                                             \
    _freeEmpty_##LIST_TYPE##_(LIST_PTR);                          \
  } while (0)

/**
 * @brief Inserts a new node behind a specified node in an intrusive list.
 *
 * This macro inserts NEW_NODE_PTR immediately after NODE_PTR in the list.
 * The list's size counter will be automatically updated.
 *
 * @param LIST_PTR      Pointer to the intrusive list structure.
 * @param NODE_PTR      Pointer to the existing node after which to insert.
 * @param NEW_NODE_PTR  Pointer to the new node to be inserted.
 *
 * @warning The NEW_NODE_PTR must remain valid for the entire duration of its
 *          membership in the list. Inserting stack-allocated nodes or nodes
 *          with limited lifetime will cause UNDEFINED BEHAVIOR.
 * @warning NODE_PTR must be a valid node currently in the list.
 * @warning NEW_NODE_PTR must not already be part of any list.
 *
 * @note This operation is O(1) time complexity.
 * @note The list's size counter will be incremented by 1.
 *
 * Example usage (with proper lifetime management):
 * @code
 * IntrusiveList list;
 * IntrusiveNode* existing_node = ...;
 *
 * // Safe insertion with heap-allocated node
 * IntrusiveNode* new_node = MAKE_NODE(CharNode, node);
 * INSERT_BEHIND(&list, existing_node, new_node);
 *
 * // WARNING: UNSAFE pattern (stack-allocated node)
 * // IntrusiveNode stack_node;
 * // INSERT_BEHIND(&list, existing_node, &stack_node); // DANGEROUS!
 * @endcode
 */
#define INSERT_BEHIND(LIST_PTR, NODE_PTR, NEW_NODE_PTR) \
  do {                                                  \
    ((NEW_NODE_PTR)->prev) = (NODE_PTR);                \
    ((NEW_NODE_PTR)->next) = ((NODE_PTR)->next);        \
    (((NODE_PTR)->next)->prev) = (NEW_NODE_PTR);        \
    ((NODE_PTR)->next) = (NEW_NODE_PTR);                \
    ((LIST_PTR)->size)++;                               \
  } while (0)

/**
 * @brief Inserts a new node in front of a specified node in an intrusive list.
 *
 * This macro inserts NEW_NODE_PTR immediately before NODE_PTR in the list.
 * The list's size counter will be automatically updated.
 *
 * @param LIST_PTR      Pointer to the intrusive list structure (must have
 * 'size' member)
 * @param NODE_PTR      Pointer to the existing node before which to insert
 * (must be in list)
 * @param NEW_NODE_PTR  Pointer to the new node to insert (must not be in any
 * list)
 *
 * @warning NEW_NODE_PTR must have a lifetime that exceeds its list membership.
 *          Inserting stack-allocated or temporary nodes causes UNDEFINED
 * BEHAVIOR.
 * @warning NODE_PTR must be a valid node currently in the list.
 * @warning NEW_NODE_PTR must not already belong to any list.
 * @warning The LIST_PTR must point to a valid initialized list structure.
 *
 * @note Time complexity is O(1)
 * @note The list's size counter will be incremented by 1
 * @note For thread safety, proper synchronization is required
 *
 * Example (safe usage):
 * @code
 * IntrusiveList list;
 * IntrusiveNode* existing = list.head;
 *
 * // Safe heap allocation
 * IntrusiveNode* new_node = malloc(sizeof(IntrusiveNode));
 * INIT_NODE(new_node);
 * INSERT_IN_FRONT_OF(&list, existing, new_node);
 *
 * // UNSAFE examples:
 * // IntrusiveNode stack_node;  // Stack allocation
 * // INSERT_IN_FRONT_OF(&list, existing, &stack_node); // WRONG!
 * @endcode
 */
#define INSERT_IN_FRONT_OF(LIST_PTR, NODE_PTR, NEW_NODE_PTR) \
  do {                                                       \
    ((NEW_NODE_PTR)->prev) = ((NODE_PTR)->prev);             \
    ((NEW_NODE_PTR)->next) = (NODE_PTR);                     \
    (((NODE_PTR)->prev)->next) = (NEW_NODE_PTR);             \
    ((NODE_PTR)->prev) = (NEW_NODE_PTR);                     \
    ((LIST_PTR)->size)++;                                    \
  } while (0)

/**
 * @brief Initializes an intrusive list structure.
 *
 * This macro initializes a list structure by calling the type-specific
 * initialization function _init_##LIST_TYPE##_(). The actual initialization
 * logic is implemented in this dependent function.
 *
 * @param LIST_TYPE The type of the list structure to initialize.
 * @param LIST_PTR  Pointer to the list structure to be initialized.
 *
 * @pre Must have corresponding _init_##LIST_TYPE##_() function defined.
 * @post The list will be in valid empty state (size = 0, head/tail properly
 * set).
 *
 * @warning The LIST_TYPE must match exactly with the existing initialization
 *          function name pattern _init_##LIST_TYPE##_().
 * @warning The LIST_PTR must point to allocated memory (stack or heap).
 * @warning Never call this on an already initialized list.
 *
 * @see _init_##LIST_TYPE##_()  The actual initialization function being called.
 *
 * Example usage:
 * @code
 * // Requires _init_IntrusiveList_() function to be defined elsewhere
 * IntrusiveList my_list;
 * INIT_LIST(IntrusiveList, &my_list);  // Properly initializes the list
 *
 * // Implementation requirement example:
 * // void _init_IntrusiveList_(IntrusiveList* list) {
 * //     list->size = 0;
 * //     list->head = list->tail = NULL;
 * // }
 * @endcode
 */
#define INIT_LIST(LIST_TYPE, LIST_PTR) (_init_##LIST_TYPE##_(LIST_PTR))

/**
 * @brief Intrusive linked list node structure.
 *
 * This structure serves as the base node for intrusive linked list
 * implementations. It contains previous and next pointers to form a
 * doubly-linked list.
 *
 * @note To use intrusive lists:
 * 1. Embed this structure inside your data structure
 * 2. Use CONTAINER_OF() to convert between node and container
 * 3. Manage memory lifetime carefully
 *
 * @warning Nodes should be initialized before use (via INIT_NODE or manually)
 * @warning Nodes can only belong to one list at a time
 *
 * Example usage:
 * @code
 * struct my_data {
 *     int value;
 *     IntrusiveNode node;  // Embedded node
 * };
 *
 * // Get containing structure from node
 * struct my_data* data = CONTAINER_OF(struct my_data, node, node_ptr);
 * @endcode
 */
typedef struct IntrusiveNode {
  struct IntrusiveNode
      *prev; /**< Pointer to previous node in list (NULL if head) */
  struct IntrusiveNode
      *next; /**< Pointer to next node in list (NULL if tail) */

} IntrusiveNode;

/**
 * @brief Doubly-linked list structure using intrusive nodes.
 *
 * This structure implements a doubly-linked list header containing pointers to
 * the head and tail nodes, plus a size counter for O(1) length queries.
 *
 * @note The list maintains the following invariants:
 * 1. head->prev is always NULL
 * 2. tail->next is always NULL
 * 3. size always matches actual node count plus 2
 * 4. Empty list has head->next = tail and size=0
 *
 * @warning Modifying head/tail/size manually may break list integrity.
 *          Always use provided operations (INSERT, REMOVE, etc.)
 *
 * Example usage:
 * @code
 * DoublyLinkedList list;
 * INIT_LIST(DoublyLinkedList, &list);
 *
 * IntrusiveNode* node = MAKE_NODE(MyData, node_field);
 * INSERT_BEHIND(&list, list.head, node);
 * @endcode
 */
typedef struct DoublyLinkedList {
  IntrusiveNode *head; /**< Pointer to first node (NULL if empty) */
  IntrusiveNode *tail; /**< Pointer to last node (NULL if empty) */
  size_t size; /**< Number of nodes in list (maintained automatically) */
} DoublyLinkedList;

/**
 * @brief Circular linked list structure using intrusive nodes.
 *
 * This structure implements a circular linked list header where the last node
 * points back to the head, forming a ring. The list maintains a size counter
 * for O(1) length queries.
 *
 * @note The list maintains the following invariants:
 * 1. In non-empty list: head->prev points to tail (circular)
 * 2. In non-empty list: tail->next points to head (circular)
 * 3. size always matches actual node count plus 1
 * 4. Empty list has head->next = head and size=0
 *
 * @warning Modifying head/size manually may break list integrity.
 *          Always use provided operations (INSERT, REMOVE, etc.)
 *
 * Example usage:
 * @code
 * CircularLinkedList list;
 * INIT_LIST(CircularLinkedList, &list);
 *
 * IntrusiveNode* node = MAKE_NODE(MyData, node_field);
 * INSERT_BEHIND(&list, list.head, node);
 * @endcode
 */
typedef struct CircularLinkedList {
  IntrusiveNode *head;
  size_t size;
} CircularLinkedList;

void _init_DoublyLinkedList_(DoublyLinkedList *listPtr);

void _init_CircularLinkedList_(CircularLinkedList *listPtr);

void _freeEmpty_DoublyLinkedList_(DoublyLinkedList *listPtr);

void _freeEmpty_CircularLinkedList_(CircularLinkedList *listPtr);

void _init_DoublyLinkedList_(DoublyLinkedList *listPtr) {
  IntrusiveNode *headNode = NULL, *tailNode = NULL;
  headNode = (IntrusiveNode *)malloc(sizeof(IntrusiveNode));
  tailNode = (IntrusiveNode *)malloc(sizeof(IntrusiveNode));

  if (headNode == 0 || tailNode == 0) {
    perror("Init doubly linked list failed!");
    free(tailNode);
    free(headNode);
    return;
  }

  headNode->next = tailNode;
  headNode->prev = NULL;
  tailNode->next = NULL;
  tailNode->prev = headNode;

  listPtr->size = 0;
  listPtr->head = headNode;
  listPtr->tail = tailNode;
}

void _init_CircularLinkedList_(CircularLinkedList *listPtr) {
  IntrusiveNode *headNode = 0;
  headNode = (IntrusiveNode *)malloc(sizeof(IntrusiveNode));

  if (headNode == 0) {
    perror("Init circular linked list failed!");
    free(headNode);
    return;
  }

  headNode->prev = headNode;
  headNode->next = headNode;

  listPtr->head = headNode;
  listPtr->size = 0;
}

void _freeEmpty_DoublyLinkedList_(DoublyLinkedList *listPtr) {
  assert(listPtr->size == 0);
  free(listPtr->tail);
  free(listPtr->head);
  listPtr->tail = NULL;
  listPtr->head = NULL;
}

IntrusiveNode *_remove_Node_WithoutChangingSize(IntrusiveNode *nodePtr) {
  nodePtr->prev->next = nodePtr->next;
  nodePtr->next->prev = nodePtr->prev;
  nodePtr->prev = 0;
  nodePtr->next = 0;
  return nodePtr;
}

void _freeEmpty_CircularLinkedList_(CircularLinkedList *listPtr) {
  assert(listPtr->size == 0);
  free(listPtr->head);
  listPtr->head = NULL;
}

// Specific data structure

typedef struct CharNode {
  char dat;
  IntrusiveNode node;
} CharNode;

// Algorithm

#include <string.h>

// 获取输入并根据输入生成链表
void initAndGenList(CircularLinkedList *lst) {
  INIT_LIST(CircularLinkedList, lst);
  char chs[1000];
  scanf("%s", chs);

  size_t len = strlen(chs);
  for (size_t i = 0; i < len; i++) {
    IntrusiveNode *newNode = MAKE_NODE(CharNode, node);
    CONTAINER_OF(CharNode, node, newNode)->dat = chs[i];
    INSERT_IN_FRONT_OF(lst, lst->head, newNode);
  }
}

// 反转链表
void reverseList(CircularLinkedList *lst) {
  SequenceStack stack;
  INIT_STACK(char, &stack);

  IntrusiveNode *node = lst->head->next;

  while (node != lst->head) {
    char ch = CONTAINER_OF(CharNode, node, node)->dat;

    // 字符入栈
    STACK_PUSH(char, &stack, ch);
    IntrusiveNode *oldNode = node;
    node = node->next;
    REMOVE_AND_RELEASE(CharNode, node, lst, oldNode);
  }

  while (stack.size) {
    // 字符出栈
    char ch = STACK_TOP(char, &stack);
    STACK_POP(char, &stack);
    IntrusiveNode *newNode = MAKE_NODE(CharNode, node);
    CONTAINER_OF(CharNode, node, newNode)->dat = ch;
    INSERT_IN_FRONT_OF(lst, lst->head, newNode);
  }
}

// 打印链表, 像字符串那样
void printCharList(CircularLinkedList *lst) {
  IntrusiveNode *node = lst->head->next;

  while (node != lst->head) {
    char ch = CONTAINER_OF(CharNode, node, node)->dat;
    putchar(ch);
    node = node->next;
  }

  putchar('\n');
}

int main() {
  CircularLinkedList lst;

  initAndGenList(&lst);

  // 初始链表状态
  printCharList(&lst);

  // 通过栈完成反转
  reverseList(&lst);

  // 终止状态
  printCharList(&lst);
}
