#include <stdio.h>
#include <string.h>

int bf(const char *pattern, char *src) {
  size_t patternLen = strlen(pattern);
  size_t srcLen = strlen(src);

  size_t res = 0;
  for (size_t i = 0; i <= srcLen - patternLen; i++) {
    if (!strncmp(src + i, pattern, patternLen)) {
      res = i + 1;
    }
  }
  return res;
}

int main() {
  char str[1000] = {0}, pattern[100] = {0};
  scanf("%s %s", str, pattern);
  size_t res = bf(pattern, str);
  printf("%ld\n", res);
}
