#include <iterator>
#include <list>
#include <iostream>

template <typename T>
void removeRepeatedELems(std::list<T> &lst) {
  for (auto it = lst.begin(); it != lst.end(); it = std::next(it)) {
    while (std::next(it) != lst.end() && *it == *std::next(it)) {
      lst.erase(std::next(it));
    }
  }
}

int main () {
  std::list<int> lst = {1, 2, 2, 2, 3, 5, 5};
  removeRepeatedELems(lst);
  for (auto it = lst.begin(); it != lst.end(); it++) {
    std::cout << *it;
    if (std::next(it) != lst.end()) {
      std::cout << ", ";
    }
  }
  std::cout << '\n';
}
