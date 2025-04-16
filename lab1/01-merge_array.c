#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Merge two ordered sequence int lists to one sequence list.
 *
 * This function maintained two ptrs and add the smaller one to the new list.
 *
 * @param dest the destination array
 * @param src1 the first sequence list
 * @param len1 the length of the ordered sequence list.
 * @param src2 the second sequence list.
 * @param len2 the length of the second list
 */
void mergeList(int *dest, int *src1, size_t len1, int *src2, size_t len2);

int main(void) {
  size_t n, m;
  scanf("%ld", &n);
  int *arr1 = (int *)(malloc(sizeof(int) * n));
  for (int i = 0; i < n; i++) {
    scanf("%d", &arr1[i]);
  }
  scanf("%ld", &m);
  int *arr2 = (int *)malloc(sizeof(int) * m);
  for (int i = 0; i < m; i++) {
    scanf("%d", &arr2[i]);
  }
  int *res = (int *)malloc(sizeof(int) * (n + m));
  mergeList(res, arr1, n, arr2, m);
  for (int i = 0; i < n + m; i++) {
    printf("%d\n", res[i]);
  }
}

void mergeList(int *dest, int *src1, size_t len1, int *src2, size_t len2) {
  // Get the begin ptr and end ptr of the lists.
  int *src1Begin = src1, *src1End = src1 + len1;
  int *src2Begin = src2, *src2End = src2 + len2;
  int *destBegin = dest, *destEnd = dest + (len1 + len2);

  // Get the starting iterators of the first list, the second list and the list
  // of destinaiton.
  int *ptr1 = src1Begin, *ptr2 = src2Begin, *ptrDest = destBegin;

  // Traverse both lists.
  while (ptr1 != src1End && ptr2 != src2End) {
    // If the value of the first iterator is less than the one in the second
    // iterator, then add the number in the first iterator to the destinaiton
    // and move the first iterator and the dest iterator to the next positoon.
    if (ptr1[0] <= ptr2[0]) {
      ptrDest[0] = ptr1[0];
      ptr1++;
    }
    // Else add the value of the second iterator to the dest list.
    else {
      ptrDest[0] = ptr2[0];
      ptr2++;
    }
    ptrDest++;
  }

  // Move the rest part of the lists to the end of the dest list.
  if (ptr1 != src1End) {
    memcpy(ptrDest, ptr1, (char *)(src1End) - (char *)(ptr1));
  } else if (ptr2 != src2End) {
    memcpy(ptrDest, ptr2, (char *)(src2End) - (char *)(ptr2));
  } else {
    assert(0);
  }
}
