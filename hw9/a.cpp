#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

namespace haomi {
  size_t countFind(auto &&arr, auto val) {
    auto size = arr.size();
    size_t left = 0, right = size - 1;
    size_t mid = (left + right) / 2;
    
    size_t res{0};
    while (left <= right) {
      res++;
      if (val == arr[mid]) {
        return res;
      }
      
      if (val < arr[mid]) {
        right = mid - 1;
        mid = (left + right) / 2;
      } else {
        left = mid + 1;
        mid = (left + right) / 2;
      }
    }
    return SIZE_MAX;
  }
}

int main() {
  std::vector<int> v{1, 2, 3, 4, 5};
  std::cout << haomi::countFind(v, 1);
}
