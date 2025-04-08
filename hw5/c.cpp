#include <cassert>
#include <cstddef>
#include <iostream>
#include <ostream>
#include <vector>

template <typename T>
std::vector<T> modifyPositionOfElements(const std::vector<T> &src, const std::vector<size_t> pos) {
  assert(src.size() == pos.size());
  std::vector<T> res(src.size());
  for (size_t i = 0; i < pos.size(); i++) {
    size_t j = pos[i];
    res[i] = src[j];
  }
  return res;
}

int main() {
  size_t n = 5;
  
  std::vector<int> a = {1, 2, 3, 4, 5};
  std::vector<size_t> b = {2, 3, 4, 1, 0};
  // The result should be {3, 4, 5, 2, 1} since:
  // Move a[2] to a[0]
  // Move a[3] to a[1]
  // Move a[4] to a[2]
  // Move a[1] to a[3]
  // Move a[0] to a[4]


  auto res = modifyPositionOfElements(a, b);
  for (auto i : res) {
    std::cout << i << std::endl;
  }
}
