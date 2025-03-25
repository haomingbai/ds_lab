#include <queue>
#include <string>
#include <iostream>

// Index begin with 0
template <typename Container>
auto processArray(Container &arr) {
  auto x = arr[0];
  std::queue<decltype(x)> qEven, qOdd;

    for (size_t i = 0; i < arr.size(); i++) {
      if (i % 2 == 0) {
        qEven.push(arr[i]);
      } else {
        qOdd.push(arr[i]);
      }
    }

    size_t index = 0;
    while (!qEven.empty()) {
      arr[index] = qEven.front();
      qEven.pop();
      index++;
    }
    while (!qOdd.empty()) {
      arr[index] = qOdd.front();
      qOdd.pop();
      index++;
    }
}

int main() {
  std::string str;
  std::cin >> str;
  processArray(str);
  std::cout << str << std::endl;
}
