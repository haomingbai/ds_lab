#include <cstddef>
#include <iostream>
#include <ostream>
#include <vector>

template <typename T>
size_t findLastIndexOfMaxElement(std::vector<T> &vec) {
  if (vec.empty()) {
    return 0;
  }
  size_t idx = 0;
  T val = vec[0];
  for (size_t i = 0; i < vec.size(); i++) {
    if (val > vec[i]) {
      idx = i;
      val = vec[i];
    } else if (val == vec[i]) {
      idx = i;
    }
  }
  return idx;
}

int main() {
  std::vector <int> v;
  for (int i = 0; std::cin >> i;) {
    v.push_back(i);
  }
  size_t idx = findLastIndexOfMaxElement(v);
  std::cout << idx << std::endl;
}
