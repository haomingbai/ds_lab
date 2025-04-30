#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define swap(a, b) \
  do {             \
    typeof(a) swp; \
    swp = a;       \
    a = b;         \
    b = swp;       \
  } while (0)

void str_reverse(char *str, size_t len) {
  if (len > 1) {
    swap(str[0], str[len - 1]);
    str_reverse(str + 1, len - 2);
  }
}

int main() {
  char buf[1000];
  scanf("%s", buf);
  str_reverse(buf, strlen(buf));
  printf("%s\n", buf);
}
