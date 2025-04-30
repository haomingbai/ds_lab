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
    auto it1 = begin, it2 = target.begin();
    for (; it2 != target.end(); it1++, it2++) {
      if (it1 == end) {
        flag = false;
        break;
      }
      if (*it1 != *it2) {
        flag = false;
        break;
      }
    }
    if (flag == true) {
      return 1 + find_max_occ(it1, end, target);
    } else {
      return find_max_occ(++begin, end, target);
    }
  }
}

int main() {
  list<char> src = {'a', 'a', 'b', 'a', 'b', 'a', 'd'},
             target = {'a', 'b', 'a'};
  auto res = find_max_occ(src.begin(), src.end(), target);
  cout << res << endl;
}
