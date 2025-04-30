#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <ostream>
#include <vector>

using namespace std;

auto maxVal(auto begin, auto end, auto &&min) {
  if (end - begin == 0) {
    return min;
  }
  if (end - begin == 1) {
    return *begin;
  }
  size_t len = (end - begin);
  auto it = begin + len / 2;
  return max(maxVal(begin, it, min), maxVal(it, end, min));
}

int main() {
  vector<int> v = {1, 4, 6, 3, 7, 8, 3, 5, 4};
  auto x = maxVal(v.begin(), v.end(), 0);
  cout << x << endl;
}
