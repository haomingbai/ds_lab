#include <iostream>
#include <iterator>
#include <ostream>
#include <utility>
#include <vector>

#define use using

template <typename T>
use vec = std::vector<T>;

use i32 = int;
use i64 = long long;
use i16 = short;
use i8 = signed char;
use isize = signed long;
use u8 = unsigned char;
use u16 = unsigned short;
use u32 = unsigned int;
use u64 = unsigned long long;
use usize = unsigned long;

void lCSort(auto begin, auto end);

void rCSort(auto begin, auto end) {
  if (begin == end || std::next(begin) == end) {
    return;
  }
  for (auto cIt = std::prev(end); cIt != begin; cIt = std::prev(cIt)) {
    if (*std::prev(cIt) > *cIt) {
      std::swap(*cIt, *std::prev(cIt));
    }
  }
  lCSort(std::next(begin), end);
}

void lCSort(auto begin, auto end) {
  if (begin == end || std::next(begin) == end) {
    return;
  }
  for (auto cIt = begin; std::next(cIt) != end; cIt = std::next(cIt)) {
    if (*std::next(cIt) < *cIt) {
      std::swap(*cIt, *std::next(cIt));
    }
  }
  rCSort(begin, std::prev(end));
}

void cSort(auto begin, auto end) { lCSort(begin, end); }

i32 main() {
  vec<i32> v{3, 5, 1, 4, 2};
  // 这里用迭代器的模式指定范围
  cSort(v.begin(), v.end());
  for (auto it : v) {
    std::cout << it << std::endl;
  }
}
