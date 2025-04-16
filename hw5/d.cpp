#include <algorithm>
#include <cstddef>
#include <iostream>
#include <ostream>
#include <vector>

template <typename T>
bool transpose(std::vector<std::vector<T>> &mat) {
  size_t row_num = mat.size();
  for (auto &it : mat) {
    if (it.size() != row_num) {
      return false;
    }
  }

  for (size_t i = 0; i < row_num; i++) {
    for (size_t j = i + 1; j < row_num; j++) {
      // Switch the two vars
      mat[i][j] ^= mat[j][i] ^= mat[i][j] ^= mat[j][i];
    }
  }
  return true;
}

int main() {
  using namespace std;
  vector<vector<int>> mat = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
  bool r =transpose(mat);

  cout << "result: " << (r ? "true" : "false") << endl;
  for (size_t i = 0; i < mat.size(); i++) {
    for (size_t j = 0; j < mat[i].size(); j++) {
      cout << mat[i][j] << ' ';
    }
    cout << endl;
  }
}
