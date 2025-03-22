#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Intrusive list data structure

/**
 * @brief Macro to get the pointer to the containing structure from a pointer to
 * one of its members.
 *
 * This macro calculates the address of the containing structure from the
 * address of one of its members.
 *
 * @param nodePtr Pointer to the member within the containing structure.
 * @param nodeType Type of the containing structure.
 * @param member Name of the member within the containing structure.
 *
 * @return Pointer to the containing structure.
 */
#define containerOf(nodePtr, nodeType, member) \
  ((nodeType *)((unsigned char *)(nodePtr) - offsetof(nodeType, member)))

/**
 * @brief Macro to remove a node from a linked list and release its memory.
 *
 * This macro removes a node from a linked list and releases the memory
 * allocated for the node. It takes four parameters:
 *
 * @param listPtr Pointer to the head of the linked list.
 * @param nodePtr Pointer to the node to be removed.
 * @param nodeType Type of the node structure.
 * @param member Name of the member in the node structure that points to the
 * next node.
 */
#define removeAndRelease(listPtr, nodePtr, nodeType, member) \
  (free(containerOf(removeNode(listPtr, nodePtr), nodeType, member)))

/**
 * @brief Macro to create a new node of a specified type and member.
 *
 * This macro is used to dynamically allocate memory for a new node of the given
 * type and initialize its member.
 *
 * @param nodeType The type of the node to be created.
 * @param member The member to be initialized in the newly created node.
 */
#define makeNode(nodeType, member) \
  (&((nodeType *)(malloc(sizeof(nodeType))))->member)

/**
 * @brief Macro to destroy a list and free its memory.
 *
 * This macro iterates through a list and frees the memory allocated for each
 * element.
 *
 * @param listPtr Pointer to the head of the list.
 * @param type The type of the elements in the list.
 * @param member The member of the list structure that points to the next
 * element.
 */
#define destroyList(listPtr, type, member)                            \
  do {                                                                \
    while ((listPtr)->head->next != (listPtr)->tail) {                \
      removeAndRelease(listPtr, (listPtr)->head->next, type, member); \
    }                                                                 \
    (listPtr)->length = 0;                                            \
    free((listPtr)->head);                                            \
    free((listPtr)->tail);                                            \
  } while (0)

/**
 * @file 02-pi.c
 * @brief This file contains the definition of the IntrusiveNode structure.
 */
struct IntrusiveNode;

/**
 * @file 02-pi.c
 * @brief This file contains the definition of the DoubleLinkedList structure.
 */
struct DoubleLinkedList;

/**
 * @typedef IntrusiveNode
 * @brief A type definition for the IntrusiveNode structure.
 */
typedef struct IntrusiveNode IntrusiveNode;

/**
 * @typedef DoubleLinkedList
 * @brief A typedef for a double linked list structure.
 */
typedef struct DoubleLinkedList DoubleLinkedList;

/**
 * @brief Inserts a node behind the specified destination node in a double
 * linked list.
 *
 * @param list Pointer to the double linked list.
 * @param destination Pointer to the node after which the new node will be
 * inserted.
 * @param newNode Pointer to the new node to be inserted.
 */
void insertBehind(DoubleLinkedList *list, IntrusiveNode *destination,
                  IntrusiveNode *target);

/**
 * @brief Inserts a node in front of the specified destination node in a double
 * linked list.
 *
 * @param list Pointer to the double linked list.
 * @param destination Pointer to the node in front of which the new node will be
 * inserted.
 */
void insertInFrontOf(DoubleLinkedList *list, IntrusiveNode *destination,
                     IntrusiveNode *target);

/**
 * @brief Removes a node from a double linked list.
 *
 * This function removes the specified node from the given double linked list.
 *
 * @param list A pointer to the double linked list from which the node will be
 * removed.
 * @param targer A pointer to the node that will be removed from the list.
 * @return A pointer to the removed node, whose data might be still useful. If
 * you want to release the memory, you should use removeAndRelease() instead.
 */
IntrusiveNode *removeNode(DoubleLinkedList *list, IntrusiveNode *targer);

/**
 * @brief Initialize the list by assigning a head node and tail node, which can
 * serve as "begin" and "rbegin" like the C++ code
 *
 * @param The pointer of the list to be initialized.
 */
void initList(DoubleLinkedList *list);

/**
 * @brief Repair the length of the linked list
 *
 * @param The pointer of the list whose lenth is not worth trust.
 */
size_t getLength(DoubleLinkedList *list);

struct IntrusiveNode {
  IntrusiveNode *prev, *next;
};

struct DoubleLinkedList {
  IntrusiveNode *head, *tail;
  size_t length;
};

void insertBehind(DoubleLinkedList *list, IntrusiveNode *destination,
                  IntrusiveNode *target) {
  /* Make sure that the destination is not the tail of the list. */
  assert(destination != list->tail);

  /* Modify the pointer of the target, which is the node to be inserted. */
  target->prev = destination;
  target->next = destination->next;

  /* Modify the pointer of the near by nodes. */
  destination->next->prev = target;
  destination->next = target;

  /* Modify the length of the list */
  list->length++;
}

void insertInFrontOf(DoubleLinkedList *list, IntrusiveNode *destination,
                     IntrusiveNode *target) {
  /* Make sure that the destination is not the tail of the list. */
  assert(destination != list->head);

  /* Modify the pointer of the target, which is the node to be inserted. */
  target->next = destination;
  target->prev = destination->prev;

  /* Modify the pointer of the near by nodes. */
  destination->prev->next = target;
  destination->prev = target;

  /* Modify the length of the list */
  list->length++;
}

IntrusiveNode *removeNode(DoubleLinkedList *list, IntrusiveNode *target) {
  // Modify the near by nodes.
  target->prev->next = target->next;
  target->next->prev = target->prev;

  // Get the target node out of the list.
  target->prev = target;
  target->next = target;

  // Check if there exists errors.
  assert(list->length > 0);

  // Modify the length of the list.
  list->length--;

  // The target ptr might be useful, the return can serve as a tool for the
  // "removeAndRelease" macro.
  return target;
}

void initList(DoubleLinkedList *list) {
  // The head and tail node should be a constant before the list is destroyed.
  list->head = (IntrusiveNode *)malloc(sizeof(IntrusiveNode));
  list->tail = (IntrusiveNode *)malloc(sizeof(IntrusiveNode));

  // Make both nodes point to each other and the outer pointers to point at
  // NULL.
  list->head->prev = NULL, list->head->next = list->tail,
  list->tail->next = NULL, list->tail->prev = list->head;

  // The length of head and tail is not taken into consideration for they cannot
  // store data.
  list->length = 0;
}

size_t getLength(DoubleLinkedList *list) {
  // Starts from the first element.
  IntrusiveNode *node = list->head->next;
  size_t size = 0;

  // Once the node reaches the tail, end the loop.
  while (node != list->tail) {
    node = node->next;
    size++;
  }

  // Set the length.
  list->length = size;
  return size;
}

// Specific data structure and functions

/**
 * @brief Structure representing a node containing a character.
 */
