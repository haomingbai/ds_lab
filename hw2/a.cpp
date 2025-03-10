#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <ostream>
#include <utility>

namespace dshw2 {
template <typename T>
class seqList {
  T *data;
  size_t len;
  size_t capacity;

 public:
  seqList() : len(0), capacity(1), data((T *)malloc(sizeof(T))) {}

  void insert(T &val) {
    for (size_t i = 0; i < len; i++) {
      if (val < data[i]) {
        if (len == capacity) {
          capacity = std::max(size_t(1), capacity << 1);
          data = (T *)realloc((void *)data, sizeof(T) * capacity);
        }
        memmove(data + i + 1, data + i, sizeof(T) * (len - i));
        data[i] = val;
        len++;
        return;
      }
    }
    if (len == capacity) {
      capacity = std::max(size_t(1), capacity << 1);
      data = (T *)realloc((void *)data, sizeof(T) * capacity);
    }
    data[len] = val;
    len++;
    return;
  }

  void insert(T &&val) {
    for (size_t i = 0; i < len; i++) {
      if (val < data[i]) {
        if (len == capacity) {
          capacity = std::max(size_t(1), capacity << 1);
          data = (T *)realloc((void *)data, sizeof(T) * capacity);
        }
        memmove(data + i + 1, data + i, sizeof(T) * (len - i));
        data[i] = std::move(val);
        len++;
        return;
      }
    }
    if (len == capacity) {
      capacity = std::max(size_t(1), capacity << 1);
      data = (T *)realloc((void *)data, sizeof(T) * capacity);
    }
    data[len] = std::move(val);
    len++;
    return;
  }

  friend std::ostream &operator<<(std::ostream &out, seqList &lst) {
    out << '[';
    for (size_t i = 0; i < lst.len; i++) {
      out << lst.data[i];
      if (i != lst.len - 1) {
        out << ',';
      }
    }
    out << ']';
    return out;
  }
};
}  // namespace dshw2

int main() {
  dshw2::seqList<int> lst;
  lst.insert(1);
  lst.insert(2);
  lst.insert(3);
  lst.insert(4);
  lst.insert(5);
  lst.insert(7);
  std::cout << lst << std::endl;
  lst.insert(6);
  std::cout << lst << std::endl;
}
