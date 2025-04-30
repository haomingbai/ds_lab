#include <cstddef>
#include <iostream>
#include <list>
#include <ostream>

using namespace std;

size_t find_max_occ(auto begin, auto end, auto target) {
  if (begin == end) {
    return 0;
  } else {
    bool flag = true;
    for (auto it1 = begin, it2 = target.begin(); it2 != target.end();
         it1++, it2++) {
      if (it1 == end) {
        flag = false;
        break;
      }
      if (*it1 != *it2) {
        flag = false;
        break;
      }
    }
    return (size_t)flag + find_max_occ(++begin, end, target);
  }
}

int main() {
  list<char> src = {'a', 'a', 'b', 'a', 'b', 'a', 'd'},
             target = {'a', 'b', 'a'};
  auto res = find_max_occ(src.begin(), src.end(), target);
  cout << res << endl;
}
