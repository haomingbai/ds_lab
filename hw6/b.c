#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int find_last_occurence(const char *str, size_t len, char target) {
  if (!len) {
    return -1;
  } else if (str[len - 1] == target) {
    return len - 1;
  } else {
    return find_last_occurence(str, len - 1, target);
  }
}

int main() {
  char *buf = malloc(sizeof(char) * 1000);
  char tar;
  scanf("%s %c", buf, &tar);
  int res = find_last_occurence(buf, strlen(buf), tar);
  printf("%d\n", res);
  return 0;
}
