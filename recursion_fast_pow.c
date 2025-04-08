#include <stdio.h>
unsigned long long fast_pow(unsigned long long base, unsigned long long factor,
                            unsigned long long mod) {
  if (factor == 0) {
    return 1;
  } else if (factor == 1) {
    return base % mod;
  }

  unsigned long long powed = fast_pow(base, factor >> 1, mod);

  unsigned long long res = (powed * powed) % mod;
  if (factor % 2) {
    res *= base;
  }
  return res % mod;
}

int main() { printf("%lld\n", fast_pow(37, 87, 23)); }
