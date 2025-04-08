#include <algorithm>
#include <cstddef>
#include <iostream>
#include <ostream>
#include <vector>

template <typename T>
bool transpose(std::vector<std::vector<T>> &mat) {
  bool result = true;
  size_t m = mat.size();
  size_t n = 0;
  for (size_t i = 0; i < m; i++) {
    n = std::max(n, mat[i].size());
    if (n != m) {
      result = false;
      break;
    }
  }

  std::vector<std::vector<int>> rMat(n);
  for (auto &it : rMat) {
    it.resize(m);
  }

  for (size_t i = 0; i < m; i++) {
    for (size_t j = 0; j < mat[i].size(); j++) {
      rMat[j][i] = mat[i][j];
    }
  }
  mat = rMat;
  return result;
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
