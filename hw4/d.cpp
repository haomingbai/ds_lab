#include <iostream>
#include <iterator>
#include <ostream>
#include <string>
#include <string_view>

std::string findMaxEqualString(std::string_view str) {
  size_t sizeOfTarget = 0;
  std::string_view targetString = "";

  for (auto it = str.begin(); it != std::prev(str.end()) && it != str.end();) {
    if (*it == *std::next(it)) {
      char ch = *it;
      auto oldIt = it;
      sizeOfTarget = 0;
      while (it != str.end() && *it == ch) {
        sizeOfTarget++;
        it = std::next(it);
      }
      if (sizeOfTarget >= targetString.length()) {
        targetString = str.substr(oldIt - str.begin(), it - oldIt);
      }
    } else {
      it++;
    }
  }
  std::string ret(targetString);
  return ret;
}

int main() {
  std::string str;
  std::cin >> str;
  std::string newString = findMaxEqualString(str);
  std::cout << newString << std::endl;
}
