#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char* TinyString;

/**
 * @brief A simple algorithm of finding the first position of which the pattern
 * string occurs in the source string.
 *
 * The KMP algorithm can make pattern matching in a complexity of O(N + M), in
 * which `N` is the length of the source string while the `M` is the length of
 * the pattern string.
 *
 * @param src The original string.
 * @param pattern The pattern string. We are going to find the pattern string in
 * the source string.
 * @return The position where the pattern string was first found. -1 if the
 * value of the position does not exist.
 */
long patternMatching(const TinyString src, const TinyString pattern);

static void prefixFunction(size_t* result, const TinyString str);

static TinyString allocateString(size_t n) {
  TinyString ret = (TinyString)(malloc(sizeof(char) * (n + 1)));
  return ret;
}

static void releaseTinyString(TinyString target) { free(target); }

long patternMatching(const TinyString src, const TinyString pattern) {
  // Generate the targetString.
  size_t srcLen = strlen(src), patternLen = strlen(pattern);
  size_t targetStrLen = srcLen + patternLen + 1;

  // Allocate memory resources
  TinyString targetStr = allocateString(targetStrLen);

  // Copy and connect the string as follows:
  // {$pattern}#{$src}
  strcpy(targetStr, pattern);
  strcat(targetStr, "\n");
  strcat(targetStr, src);

  // Allocate the result array.
  size_t* pi = (size_t*)(calloc(sizeof(size_t), targetStrLen));

  // Fetch the result of pi function.
  prefixFunction(pi, targetStr);

  // Traverse the pi array.
  int res = -1;
  for (size_t i = patternLen + 1; i < targetStrLen; i++) {
    if (pi[i] >= patternLen) {
      // Release rosources.
      res = i - (patternLen * 2 + 1);
    }
  }
  free(pi);
  releaseTinyString(targetStr);
  return res;
}

static void prefixFunction(size_t* result, const TinyString str) {
  size_t strLen = strlen(str);

  result[0] = 0, result[1] = 0;

  for (size_t i = 2; i < strLen; i++) {
    size_t possibleResult = result[i - 1];

    while (possibleResult && str[i - 1] != str[possibleResult]) {
      possibleResult = result[possibleResult];
    }
    if (str[i - 1] == str[possibleResult]) {
      possibleResult++;
    }
    result[i] = possibleResult;
  }
}

int main() {
  char src[1000], pat[100];
  scanf("%s %s", src, pat);
  long pos = patternMatching(src, pat);
  pos++;  // Convert the index from 0 - n-1 to 1 - n.
  printf("%ld\n", pos);
}
