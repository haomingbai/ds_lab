#include <cstddef>
#include <iostream>
#include <ostream>
#include <utility>

template <typename T>
std::pair<T *, size_t> merge(T *a, size_t aLen, T *b, size_t bLen) {
  auto arr = new T[aLen + bLen];
  size_t arrSize = 0;
  for (size_t aPos = 0, bPos = 0; aPos < aLen && bPos < bLen;) {
    if (a[aPos] < b[bPos]) {
      if (arrSize && a[aPos] == arr[arrSize - 1]) {
        aPos++;
      } else {
        arr[arrSize++] = a[aPos++];
      }
    } else {
      if (arrSize && b[bPos] == arr[arrSize - 1]) {
        bPos++;
      } else {
        arr[arrSize++] = b[bPos++];
      }
    }
  }
  return {arr, arrSize};
}

int main() {
  int a[4] = {1, 3, 5, 7}, b[5] = {1, 2, 4, 5, 7};
  auto c = merge(a, 4, b, 5);
  for (size_t i = 0; i < c.second; i++) {
    std::cout << c.first[i] << ' ';
  }
  std::cout << std::endl;
}
