#include <algorithm>
#include <iostream>
#include <ostream>
#include <vector>

using namespace std;

template <typename T>
void move_right(vector<T> &vec, size_t step) {
  reverse(vec.begin(), vec.end());
  reverse(vec.begin() + step, vec.end());
  reverse(vec.begin(), vec.begin() + step);
}

int main() {
  vector<int> v = {1, 2, 3, 4, 5, 6};
  move_right(v, 2);
  for (auto it : v) {
    cout << it << endl;
  }
}
