#include <stdio.h>
#include <string.h>

int cntMaxPresentation(char *src) {
  size_t len = strlen(src);
  size_t cnt[256] = {0};

  for (size_t i = 0; i < len; i++) {
    cnt[src[i]]++;
  }
  
  char res = 0;

  for (char ch = 'z'; ch >= 'a'; ch--) {
    if (cnt[ch] != 0) {
      res = cnt[ch];
      break;
    }
  }
  return res;
}

int main() {
  char str[1000] = {0};

  scanf("%s", str);
  int r = cntMaxPresentation(str);

  printf("%d", r);
}
