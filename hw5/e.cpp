#include <algorithm>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>

template <typename T>
T sumDiagnoal(std::vector<std::vector<T>> &mat) {
  size_t m = mat.size(), n = 0;
  for (size_t i = 0; i < mat.size(); i++) {
    n = std::max(n, mat[i].size());
    if (m != n) {
      throw std::invalid_argument(
          "The number of rows should be equal to the number of columns!");
    }
  }

  T sum = 0;
  for (size_t i = 0; i < m; i++) {
    sum += mat[i][i];
    sum += mat[i][n - i - 1];
  }
  if (m % 2) {
    sum -= mat[(m / 2)][m / 2];
  }
  return sum;
}

template <typename T>
using matrix = std::vector<std::vector<T>>;

int main() {
  using namespace std;
  matrix<int> mat = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
  int res = sumDiagnoal(mat);
  cout << res << endl;
}
