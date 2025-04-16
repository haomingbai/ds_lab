#pragma GCC optimize(3)

#include <assert.h>
#include <math.h>
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
typedef struct CharNode {
  unsigned char dat;
  IntrusiveNode node;
} CharNode;

/**
 * @struct HighPrecision
 * @brief A structure to represent high precision numbers.
 */
typedef struct HighPrecision {
  DoubleLinkedList beforePt, afterPt;
  bool sign;
  signed char inited;
} HighPrecision;

/**
 * @brief Destroys a HighPrecision object and frees its associated memory.
 *
 * This function is responsible for properly deallocating the memory
 * associated with a HighPrecision object to prevent memory leaks.
 *
 * @param target A pointer to the HighPrecision object to be destroyed.
 */
void destroyHighPrecision(HighPrecision *target);

/**
 * @brief Initializes a HighPrecision structure.
 *
 * This function sets up the given HighPrecision structure to a default state,
 * preparing it for further operations.
 *
 * @param target A pointer to the HighPrecision structure to be initialized.
 */
void initHighPrecision(HighPrecision *target);

/**
 * @brief Sets the highest precision for a HighPrecision object.
 *
 * This function configures the highest precision of the given HighPrecision
 * object to the specified number of digits. If the precision is less than the
 * given one, then this function will do nothing.
 *
 * @param target Pointer to the HighPrecision object whose precision is to be
 * set.
 * @param precision The highest accepted precision (number of digits) to be set.
 */
void setHighestPrecision(HighPrecision *target, size_t precision);

/**
 * @brief Sets the precision for a HighPrecision object.
 *
 * This function configures the precision of the given HighPrecision
 * object to the specified number of digits.
 *
 * @param target Pointer to the HighPrecision object whose precision is to be
 * set.
 * @param precision The desired precision (number of digits) to be set.
 */
void setPrecision(HighPrecision *target, size_t precision);

/**
 * @brief Converts a string representation of a number to a high precision
 * number.
 *
 * This function takes a string representing a number and converts it into a
 * high precision number format, storing the result in the provided
 * HighPrecision structure.
 *
 * @param dest Pointer to the HighPrecision structure where the result will be
 * stored.
 * @param src  Pointer to the null-terminated string representing the number to
 * be converted.
 */
void convertStringToHighPrecision(HighPrecision *dest, const char *src);

/**
 * @brief Performs division of a numerator by a denominator and stores the
 * result in a HighPrecision structure.
 *
 * @param res Pointer to a HighPrecision structure where the result will be
 * stored.
 * @param numerator The numerator of the division.
 * @param denominator The denominator of the division.
 */
void makeDivision(HighPrecision *res, long long numerator,
                  long long denominator, size_t precision);

/**
 * @brief Adds the value of one HighPrecision number to another.
 *
 * This function takes two HighPrecision numbers and adds the value of the
 * source number (src) to the destination number (dest). The result is stored
 * in the destination number.
 *
 * @param dest Pointer to the destination HighPrecision number.
 * @param src Pointer to the source HighPrecision number.
 */
void addTo(HighPrecision *dest, HighPrecision *src);

/**
 * @brief Adds a long long integer to a HighPrecision number.
 *
 * This function takes a HighPrecision number and adds a long long integer to
 * it.
 *
 * @param dest A pointer to the HighPrecision number to which the integer will
 * be added.
 * @param src The long long integer to be added to the HighPrecision number.
 */
void addIntTo(HighPrecision *dest, long long src);

/**
 * @brief Compares two HighPrecision numbers.
 *
 * This function compares two HighPrecision numbers pointed to by `a` and `b`.
 *
 * @param a Pointer to the first HighPrecision number.
 * @param b Pointer to the second HighPrecision number.
 * @return A signed char indicating the result of the comparison:
 *         - Negative value if `a` is less than `b`
 *         - Zero if `a` is equal to `b`
 *         - Positive value if `a` is greater than `b`
 */
signed char compareHighPrecision(HighPrecision *a, HighPrecision *b);

/**
 * @brief Prints a high precision number with the specified precision.
 *
 * This function takes a pointer to a HighPrecision structure and prints
 * the number it represents with the specified number of decimal places.
 *
 * @param target Pointer to the HighPrecision structure to be printed.
 * @param precision The number of decimal places to print.
 */
void printHighPrecision(HighPrecision *target, size_t precision);

/**
 * @brief Moves the target HighPrecision point by a specified number of steps.
 *
 * This function adjusts the position of the given HighPrecision point by the
 * number of steps specified by stepNum. The exact behavior of the movement
 * depends on the implementation details of the HighPrecision type and the
 * movePoint function.
 *
 * @param target A pointer to the HighPrecision point to be moved.
 * @param stepNum The number of steps to move the target point.
 */
void movePoint(HighPrecision *target, int stepNum);

/**
 * Multiplies the value of the source HighPrecision number by the destination
 * HighPrecision number and stores the result in the destination.
 *
 * @param dest Pointer to the destination HighPrecision number where the result
 * will be stored.
 * @param src Pointer to the source HighPrecision number to be multiplied.
 * @param precision The number of decimal places to consider in the
 * multiplication.
 */
void multiplyTo(HighPrecision *dest, HighPrecision *src, size_t precision);

/**
 * @brief Macro to determine the sign of a number.
 *
 * This macro evaluates the sign of the given number `x`.
 * If `x` is greater than or equal to 0, it returns 0.
 * If `x` is less than 0, it returns 1.
 *
 * @param x The number to evaluate.
 * @return 0 if `x` is non-negative, 1 if `x` is negative.
 */
#define sign(x) (((x) >= 0) ? 0 : 1)

/**
 * @brief Macro to calculate the absolute value of a number.
 *
 * This macro calculates the absolute value of the given number `x`.
 *
 * @param x The number to calculate the absolute value of.
 * @return The absolute value of `x`.
 */
#define abs(x) (((x) >= 0) ? (x) : (-x))

void initHighPrecision(HighPrecision *target) {
  // Initialize the linked list.
  initList(&target->beforePt);
  initList(&target->afterPt);

  // Set the sign to positive.
  target->sign = 0;

  // Set the inited flag to true.
  target->inited = 1;
}

// Remove the leading zeros.
static void makeValid(HighPrecision *target) {
  while (target->beforePt.head->next != target->beforePt.tail &&
         containerOf(target->beforePt.tail->prev, CharNode, node)->dat == 0) {
    removeAndRelease(&target->beforePt, target->beforePt.tail->prev, CharNode,
                     node);
  }

  // If the value of the number is zero, the sign should be positive.
  if ((target->beforePt.length == 0) && (target->afterPt.length == 0)) {
    target->sign = 0;
  }
}

void setHighestPrecision(HighPrecision *target, size_t precision) {
  // Remove the extra digits.
  while (target->afterPt.length > precision) {
    removeAndRelease(&target->afterPt, target->afterPt.tail->prev, CharNode,
                     node);
  }
}

void setPrecision(HighPrecision *target, size_t precision) {
  // If the current precision is less than the required precision, then add
  // zeros to the end of the value.
  while (target->afterPt.length < precision) {
    IntrusiveNode *newNode;
    newNode = makeNode(CharNode, node);
    containerOf(newNode, CharNode, node)->dat = 0;
    insertInFrontOf(&target->afterPt, target->afterPt.tail, newNode);
  }

  // Or remove the extra digits.
  while (target->afterPt.length > precision) {
    removeAndRelease(&target->afterPt, target->afterPt.tail->prev, CharNode,
                     node);
  }
}

void convertStringToHighPrecision(HighPrecision *dest, const char *src) {
  // Make a copy of the string.
  size_t len = strlen(src);
  char *tmp = (char *)(malloc(sizeof(char) * (len + 1)));
  char *tmpBackup = tmp;
  strncpy(tmp, src, len);
  tmp[len] = 0;

  // Find the possible sign of the number in the string.
  while (tmp[0] < '0' || tmp[0] > '9') {
    if (tmp[0] == '-') {
      dest->sign = 1;
    }
    tmp++;
  }

  // Find the two parts of the string.
  char *strBeforePt = strtok(tmp, ".");
  char *strAfterPt = strtok(NULL, ".");
  size_t lenBeforePt = strBeforePt ? strlen(strBeforePt) : 0;
  size_t lenAfterPt = strAfterPt ? strlen(strAfterPt) : 0;

  // Set the number of different positions one by one.
  IntrusiveNode *node = dest->beforePt.head;
  if (strBeforePt != NULL && lenBeforePt) {
    while (lenBeforePt--) {
      IntrusiveNode *toInsert;
      toInsert = makeNode(CharNode, node);
      containerOf(toInsert, CharNode, node)->dat =
          strBeforePt[lenBeforePt] - '0';
      insertBehind(&dest->beforePt, node, toInsert);
      node = node->next;
    }
  }

  // Numbers after the decimal point.
  node = dest->afterPt.head;
  if (strAfterPt != NULL && lenAfterPt) {
    for (size_t i = 0; i < lenAfterPt; i++) {
      IntrusiveNode *toInsert;
      toInsert = makeNode(CharNode, node);
      containerOf(toInsert, CharNode, node)->dat = strAfterPt[i] - '0';
      insertBehind(&dest->afterPt, node, toInsert);
      node = node->next;
    }
  }

  // Remove the unnecessary zeros in front of the number.
  makeValid(dest);

  // Release resources.
  free(tmpBackup);
}

void destroyHighPrecision(HighPrecision *target) {
  // Reset the sign.
  target->sign = 0;

  // Release resources.
  destroyList(&target->beforePt, CharNode, node);
  destroyList(&target->afterPt, CharNode, node);

  // Restore the inited flag.
  target->inited = 0;
}

void makeDivision(HighPrecision *res, long long numerator,
                  long long denominator, size_t precision) {
  // Check the invalid denominators and numerators.
  if (!denominator) {
    perror("The denominator cannot be zero!");
    return;
  }
  if (!numerator) {
    return;
  }
  if (sign(numerator) == sign(denominator)) {
    res->sign = 0;
  } else {
    res->sign = 1;
  }
  numerator = abs(numerator), denominator = abs(denominator);

  // Make sure the res is newly initialized!

  // Parts before the decimal pt.
  long long beforePt = numerator / denominator;
  IntrusiveNode *node = res->beforePt.head;
  while (beforePt > 0) {
    IntrusiveNode *newNode = makeNode(CharNode, node);
    containerOf(newNode, CharNode, node)->dat = beforePt % 10;
    beforePt /= 10;
    insertBehind(&res->beforePt, node, newNode);
    node = node->next;
  }

  // Parts after the point.
  // Make division for `precision` times.
  numerator %= denominator;
  for (size_t i = 0; i < precision; i++) {
    numerator *= 10;
    int tmp;
    tmp = numerator / denominator;
    numerator %= denominator;

    IntrusiveNode *newNode;
    newNode = makeNode(CharNode, node);
    containerOf(newNode, CharNode, node)->dat = tmp;
    insertInFrontOf(&res->afterPt, res->afterPt.tail, newNode);
  }
}

void addIntTo(HighPrecision *dest, long long src) {
  // This function is currently unfinished, and can only process the positive
  // numbers or numbers with the same sign.
  HighPrecision tmpSrc;
  initHighPrecision(&tmpSrc);

  // Convert the int to a HighPrecision.
  if (src < 0) {
    src = -src;
    tmpSrc.sign = 1;
  }
  while (src > 0) {
    IntrusiveNode *newNode = makeNode(CharNode, node);
    containerOf(newNode, CharNode, node)->dat = src % 10;
    src /= 10;
    insertInFrontOf(&tmpSrc.beforePt, tmpSrc.beforePt.tail, newNode);
  }
  addTo(dest, &tmpSrc);
  destroyHighPrecision(&tmpSrc);
}

signed char compareHighPrecision(HighPrecision *a, HighPrecision *b) {
  // Compare the sign.
  if (a->sign == 0 && b->sign == 1) {
    return 1;
  } else if (a->sign == 1 && b->sign == 0) {
    return -1;
  } else {
    // Remove the extra zeros.
    makeValid(a);
    makeValid(b);

    // Fetch the common sign.
    char commonSign = a->sign ? -1 : 1;
    if (a->beforePt.length > b->beforePt.length) {
      return 1 * commonSign;
    } else if (a->beforePt.length < b->beforePt.length) {
      return -1;
    }

    // Compare the parts greater than 1.
    IntrusiveNode *aNode = a->beforePt.tail->prev,
                  *bNode = b->beforePt.tail->prev;
    while (aNode != a->beforePt.head && bNode != b->beforePt.head) {
      if (containerOf(aNode, CharNode, node)->dat >
          containerOf(bNode, CharNode, node)->dat) {
        return 1 * commonSign;
      }
      if (containerOf(aNode, CharNode, node)->dat <
          containerOf(bNode, CharNode, node)->dat) {
        return -1 * commonSign;
      }
      aNode = aNode->prev, bNode = bNode->prev;
    }

    // Compare the parts smaller than 1
    aNode = a->afterPt.head->next, bNode = b->afterPt.head->next;
    while (aNode != a->afterPt.tail && bNode != b->afterPt.tail) {
      if (containerOf(aNode, CharNode, node)->dat >
          containerOf(bNode, CharNode, node)->dat) {
        return 1 * commonSign;
      }
      if (containerOf(aNode, CharNode, node)->dat <
          containerOf(bNode, CharNode, node)->dat) {
        return -1 * commonSign;
      }
      aNode = aNode->next, bNode = bNode->next;
    }

    // If the `b` is more precise than `a`, then once the rest part of `b` is
    // not zero, than b is greater than a in absolute value.
    if (aNode == a->afterPt.tail && bNode != b->afterPt.tail) {
      bool isNotEqual = 0;
      while (bNode != b->afterPt.tail) {
        if ((containerOf(bNode, CharNode, node)->dat != 0)) {
          isNotEqual = 1;
          break;
        }
      }
      if (isNotEqual) {
        return -1 * commonSign;
      } else {
        return 0;
      }
    }  // Same when `a` is more precise.
    else if (aNode != a->afterPt.tail && bNode == b->afterPt.tail) {
      bool isNotEqual = 0;
      while (bNode != b->afterPt.tail) {
        if ((containerOf(bNode, CharNode, node)->dat != 0)) {
          isNotEqual = 1;
          break;
        }
      }
      if (isNotEqual) {
        return 1 * commonSign;
      } else {
        return 0;
      }
    }  // When they have the same precision, then `a` and `b` are the same.
    else {
      return 0;
    }
  }
}

void addTo(HighPrecision *dest, HighPrecision *src) {
  // Prevent from operating uninitialized numbers.
  if (!dest->inited || !src->inited) {
    perror("Objects uninitialited!");
    return;
  }

  // This function is currently unfinished, and can only process the positive
  // numbers or numbers with the same sign.
  bool signOfDest = dest->sign, signOfSrc = src->sign;
  dest->sign = 0, src->sign = 0;
  signed char finalSign = signOfDest;
  signed char absCmp;

  if (signOfSrc != signOfDest) {
    absCmp = compareHighPrecision(dest, src);
    if (absCmp < 0) {
      finalSign = signOfSrc;
    }
  }
  dest->sign = finalSign;
  src->sign = signOfSrc;
  if (signOfSrc == signOfDest) {
    // Get the first nude of both numbers.
    IntrusiveNode *destNode = dest->afterPt.tail->prev,
                  *srcNode = src->afterPt.tail->prev;

    // The original value of of overflow is zero.
    unsigned char overflow = 0;

    // Judge whether src is more precise than dest.
    // If it is true, then we should process with the extra digits.
    bool isSrcMorePrecise = 0;
    if (dest->afterPt.length > src->afterPt.length) {
      size_t timeOfOperation = dest->afterPt.length - src->afterPt.length;
      for (size_t i = 0; i < timeOfOperation; i++) {
        destNode = destNode->prev;
      }
    } else if (src->afterPt.length > dest->afterPt.length) {
      isSrcMorePrecise = 1;
      size_t timeOfOperation = src->afterPt.length - dest->afterPt.length;
      for (size_t i = 0; i < timeOfOperation; i++) {
        srcNode = srcNode->prev;
      }
    }

    // Prepare for further use.
    IntrusiveNode *oldSrcNode = srcNode, *oldDestNode = destNode;

    // Calculate the part at the right side of the decimal pt.
    while (destNode != dest->afterPt.head && srcNode != src->afterPt.head) {
      // Create a value.
      signed char currentResult;

      // Make addition
      currentResult = overflow;
      currentResult += (containerOf(destNode, CharNode, node)->dat +
                        containerOf(srcNode, CharNode, node)->dat);

      // Reset the overflow.
      overflow = 0;

      // Set the new value of overflow
      if (currentResult >= 10) {
        overflow = currentResult / 10;
        currentResult %= 10;
      }

      // Set the value of the node in the destination.
      containerOf(destNode, CharNode, node)->dat = currentResult;

      // Move to the previous node, or in other words, move right.
      destNode = destNode->prev, srcNode = srcNode->prev;
    }

    // Process with the upper part of the decimals.
    destNode = dest->beforePt.head->next, srcNode = src->beforePt.head->next;
    while (destNode != dest->beforePt.tail && srcNode != src->beforePt.tail) {
      signed char currentResult;
      currentResult = overflow;
      currentResult += (containerOf(destNode, CharNode, node)->dat +
                        containerOf(srcNode, CharNode, node)->dat);
      overflow = 0;
      if (currentResult >= 10) {
        overflow = currentResult / 10;
        currentResult %= 10;
      }
      containerOf(destNode, CharNode, node)->dat = currentResult;
      destNode = destNode->next, srcNode = srcNode->next;
    }

    // When the `dest` has more digits than `src.
    while (srcNode == src->beforePt.tail) {
      // If the overflow is fully processed with, exit.
      if (!overflow) {
        break;
      }

      // Add the overflow to the higher part.
      if (destNode != dest->beforePt.tail) {
        containerOf(destNode, CharNode, node)->dat += overflow;
        overflow = containerOf(destNode, CharNode, node)->dat / 10;
        containerOf(destNode, CharNode, node)->dat %= 10;
        destNode = destNode->next;
      } else if (destNode == dest->beforePt.tail) {
        // If the node has reached the tail, like "99999999" + "1", then insert
        // a new node and exit.
        IntrusiveNode *newNode = makeNode(CharNode, node);
        containerOf(newNode, CharNode, node)->dat = overflow;
        overflow = 0;
        insertInFrontOf(&dest->beforePt, dest->beforePt.tail, newNode);
        break;
      } else {
        assert(0);
      }
    }

    // If the `src` is bigger, insert new nodes to the dest.
    // Imagine 1 += 999.
    while (srcNode != src->beforePt.tail) {
      IntrusiveNode *newNode;
      newNode = makeNode(CharNode, node);

      // Process with the overflow.
      containerOf(newNode, CharNode, node)->dat =
          containerOf(srcNode, CharNode, node)->dat + overflow;

      // Reset the overflow.
      overflow = 0;

      // If overflows, then set the overflow.
      if (containerOf(newNode, CharNode, node)->dat >= 10) {
        overflow = containerOf(newNode, CharNode, node)->dat / 10;
        containerOf(newNode, CharNode, node)->dat %= 10;
      }

      // Make insertion.
      insertInFrontOf(&dest->beforePt, dest->beforePt.tail, newNode);
      srcNode = srcNode->next;

      // In some cases like 1 += 999, a new node should be added.
      if (overflow && srcNode == src->beforePt.tail) {
        IntrusiveNode *newNode;
        newNode = makeNode(CharNode, node);
        containerOf(newNode, CharNode, node)->dat = overflow;
        overflow = 0;
        insertInFrontOf(&dest->beforePt, dest->beforePt.tail, newNode);
      }
    }

    // If `src` is more precise.
    if (isSrcMorePrecise) {
      // Add the remaining part to the `dest`, which can make it more precise.
      IntrusiveNode *destNode = oldDestNode->next, *srcNode = oldSrcNode->next;
      while (srcNode != src->afterPt.tail) {
        IntrusiveNode *newNode;
        newNode = makeNode(CharNode, node);
        containerOf(newNode, CharNode, node)->dat =
            containerOf(srcNode, CharNode, node)->dat;
        insertInFrontOf(&dest->afterPt, dest->afterPt.tail, newNode);
        srcNode = srcNode->next;
      }
    }
  } else {
    IntrusiveNode *destNode, *srcNode;
    perror("Not implemented yet!");
  }
}

void printHighPrecision(HighPrecision *target, size_t precision) {
  // Sign of the HighPrecision.
  if (target->sign) {
    putchar('-');
  }

  // If the x = 0.xyz... , then print a '0' before the dot.
  if (target->beforePt.head->next == target->beforePt.tail) {
    putchar('0');
  } else {
    IntrusiveNode *node = target->beforePt.tail->prev;
    while (node != target->beforePt.head) {
      putchar(containerOf(node, CharNode, node)->dat + '0');
      node = node->prev;
    }
  }

  // Print the '.xyz...'
  if (precision > 0) {
    putchar('.');
    IntrusiveNode *node = target->afterPt.head->next;
    size_t count = 0;
    while (node != target->afterPt.tail && count < precision) {
      putchar(containerOf(node, CharNode, node)->dat + '0');
      node = node->next;
      count++;
    }
    while (count < precision) {
      putchar('0');
      count++;
    }
  }
}

static void movePointToLeft(HighPrecision *target, size_t stepNum) {
  // Prevent from operating the uninitialized targets.
  if (!target->inited) {
    perror("Target uninitialized!");
    return;
  }

  // Remove the extra numbers.
  makeValid(target);

  for (size_t hasStpped = 0; hasStpped < stepNum; hasStpped++) {
    // 123.456 to 12.3456 ...
    IntrusiveNode *currNode;
    currNode = target->beforePt.head->next;
    if (currNode == target->beforePt.tail) {
      IntrusiveNode *newNode;
      newNode = makeNode(CharNode, node);
      containerOf(newNode, CharNode, node)->dat = 0;
      insertBehind(&target->afterPt, target->afterPt.head, newNode);
    } else {
      removeNode(&target->beforePt, currNode);
      insertBehind(&target->afterPt, target->afterPt.head, currNode);
    }
  }
}

static void movePointToRight(HighPrecision *target, size_t stepNum) {
  // The reversion of the move left.
  if (!target->inited) {
    perror("Target uninitialized!");
    return;
  }

  makeValid(target);

  for (size_t hasStepped = 0; hasStepped < stepNum; hasStepped++) {
    IntrusiveNode *currNode;
    currNode = target->afterPt.head->next;
    if (currNode == target->afterPt.tail) {
      IntrusiveNode *newNode;
      newNode = makeNode(CharNode, node);
      containerOf(newNode, CharNode, node)->dat = 0;
      insertBehind(&target->beforePt, target->beforePt.head, newNode);
    } else {
      removeNode(&target->afterPt, currNode);
      insertBehind(&target->beforePt, target->beforePt.head, currNode);
    }
  }
}

void movePoint(HighPrecision *target, int stepNum) {
  // Route the move function to the right one.
  size_t absoluteStrpNum = abs(stepNum);
  if (stepNum < 0) {
    movePointToLeft(target, absoluteStrpNum);
  } else if (stepNum > 0) {
    movePointToRight(target, absoluteStrpNum);
  }
}

void multiplyTo(HighPrecision *dest, HighPrecision *src, size_t precision) {
  // Make sure the number is correct.
  if (!dest->inited || !src->inited) {
    perror("Objects uninitialized!");
    return;
  }

  // Store the sign of the result.
  bool signOfResult = (dest->sign != src->sign);
  dest->sign = 0;

  // Make a new HighPrecision as the result.
  HighPrecision result;
  initHighPrecision(&result);

  // This step is quite important,
  // which decides how to move the decimal point.
  int stepToMove = -(int)(src->afterPt.length);
  IntrusiveNode *nodeToProcess = src->afterPt.tail->prev;

  // Multiplie the dest with each digit of the src.
  while (nodeToProcess != src->afterPt.head) {
    HighPrecision currentDecimal;
    initHighPrecision(&currentDecimal);

    // Initialize the required resources.
    IntrusiveNode *nodeToCalc = dest->afterPt.tail->prev;
    unsigned char overflow, currentVal;
    overflow = 0, currentVal = 0;

    // Make multiplication digit by digit.
    while (nodeToCalc != dest->afterPt.head) {
      currentVal = overflow;
      currentVal += (containerOf(nodeToCalc, CharNode, node)->dat) *
                    (containerOf(nodeToProcess, CharNode, node)->dat);
      overflow = currentVal / 10;
      currentVal %= 10;
      IntrusiveNode *newNode;
      newNode = makeNode(CharNode, node);
      containerOf(newNode, CharNode, node)->dat = currentVal;
      insertBehind(&currentDecimal.afterPt, currentDecimal.afterPt.head,
                   newNode);
      nodeToCalc = nodeToCalc->prev;
    }

    nodeToCalc = dest->beforePt.head->next;

    while (nodeToCalc != dest->beforePt.tail) {
      currentVal = overflow;
      currentVal += (containerOf(nodeToCalc, CharNode, node)->dat) *
                    (containerOf(nodeToProcess, CharNode, node)->dat);
      overflow = currentVal / 10;
      currentVal %= 10;
      IntrusiveNode *newNode;
      newNode = makeNode(CharNode, node);
      containerOf(newNode, CharNode, node)->dat = currentVal;
      insertInFrontOf(&currentDecimal.beforePt, currentDecimal.beforePt.tail,
                      newNode);
      nodeToCalc = nodeToCalc->next;
    }

    if (overflow) {
      IntrusiveNode *newNode;
      newNode = makeNode(CharNode, node);
      containerOf(newNode, CharNode, node)->dat = overflow;
      overflow = 0;
      insertInFrontOf(&currentDecimal.beforePt, currentDecimal.beforePt.tail,
                      newNode);
    }

    // Move the decimal point to a proper place.
    movePoint(&currentDecimal, stepToMove);

    // Avoid the inreasonably high precision from slowing down the program.
    setHighestPrecision(&currentDecimal, precision);

    // Make addition.
    addTo(&result, &currentDecimal);

    // Release resources.
    destroyHighPrecision(&currentDecimal);

    // Move to the next node.
    nodeToProcess = nodeToProcess->prev;
    stepToMove++;
  }

  // Same as the previous part after the decimal point.
  nodeToProcess = src->beforePt.head->next;
  while (nodeToProcess != src->beforePt.tail) {
    HighPrecision currentDecimal;
    initHighPrecision(&currentDecimal);

    IntrusiveNode *nodeToCalc = dest->afterPt.tail->prev;
    unsigned char overflow, currentVal;
    overflow = 0, currentVal = 0;
    while (nodeToCalc != dest->afterPt.head) {
      currentVal = overflow;
      currentVal += (containerOf(nodeToCalc, CharNode, node)->dat) *
                    (containerOf(nodeToProcess, CharNode, node)->dat);
      overflow = currentVal / 10;
      currentVal %= 10;
      IntrusiveNode *newNode;
      newNode = makeNode(CharNode, node);
      containerOf(newNode, CharNode, node)->dat = currentVal;
      insertBehind(&currentDecimal.afterPt, currentDecimal.afterPt.head,
                   newNode);
      nodeToCalc = nodeToCalc->prev;
    }

    nodeToCalc = dest->beforePt.head->next;

    while (nodeToCalc != dest->beforePt.tail) {
      currentVal = overflow;
      currentVal += (containerOf(nodeToCalc, CharNode, node)->dat) *
                    (containerOf(nodeToProcess, CharNode, node)->dat);
      overflow = currentVal / 10;
      currentVal %= 10;
      IntrusiveNode *newNode;
      newNode = makeNode(CharNode, node);
      containerOf(newNode, CharNode, node)->dat = currentVal;
      insertInFrontOf(&currentDecimal.beforePt, currentDecimal.beforePt.tail,
                      newNode);
      nodeToCalc = nodeToCalc->next;
    }

    if (overflow) {
      IntrusiveNode *newNode;
      newNode = makeNode(CharNode, node);
      containerOf(newNode, CharNode, node)->dat = overflow;
      overflow = 0;
      insertInFrontOf(&currentDecimal.beforePt, currentDecimal.beforePt.tail,
                      newNode);
    }

    movePoint(&currentDecimal, stepToMove);
    setHighestPrecision(&currentDecimal, precision);
    addTo(&result, &currentDecimal);

    destroyHighPrecision(&currentDecimal);
    nodeToProcess = nodeToProcess->next;
    stepToMove++;
  }

  // Release the original resources occupied by `dest`.
  destroyHighPrecision(dest);
  *dest = result;

  // Set the correct sign.
  dest->sign = signOfResult;

  // Make the dest valid and correctly precise.
  makeValid(dest);
  setHighestPrecision(dest, precision);
}

// Final algorithm

/**
 * @brief Calculates the value of Pi to a specified precision.
 *
 * This function computes the value of Pi and stores the result in the provided
 * HighPrecision structure. The precision parameter specifies the number of
 * decimal places to which Pi should be calculated.
 *
 * @param result A pointer to a HighPrecision structure where the result will be
 * stored.
 * @param precision The number of decimal places to calculate Pi to.
 */
void calculatePi(HighPrecision *result, size_t precision) {
  initHighPrecision(result);
  size_t k = 1;
  double sum = 0;

  // Make sure how many setps should iterate.
  for (k = 1; (int)sum <= precision + 1; k++) {
    sum += log10(((double)(2 * k + 1)) / ((double)(k)));
  }

  // The most internal value, or the starting value.
  makeDivision(result, k, (2 * k + 1), precision - (k / 4) + 2);

  // Make iteration.
  while (--k) {
    addIntTo(result, 2);
    HighPrecision currentDecimal;
    initHighPrecision(&currentDecimal);
    makeDivision(&currentDecimal, k, (2 * k + 1), precision - (k / 4) + 2);
    multiplyTo(result, &currentDecimal, precision - (k / 4) + 2);
    setHighestPrecision(result, precision - (k / 4) + 2);
    destroyHighPrecision(&currentDecimal);
  }

  // Outside the result, there should be a '2' added.
  addIntTo(result, 2);

  // Maintain the precision.
  setHighestPrecision(result, precision + 2);
}

int main() {
  HighPrecision a;
  initHighPrecision(&a);
  int n;
  scanf("%d", &n);
  calculatePi(&a, n + 1);
  printHighPrecision(&a, n);
}
