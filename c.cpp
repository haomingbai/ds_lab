#include <iostream>
#include <ostream>
#include <stack>
#include <string>
#include <vector>

using namespace std;

auto chs = "0123456789abcdefg";

int main() {
  int d, r;
  cin >> d >> r;

  // 顺序栈
  stack<int, std::vector<int>> st;
  while (d) {
    st.push(d % r);
    d /= r;
  }

  string str;
  while (!st.empty()) {
    str.push_back(chs[st.top()]);
    st.pop();
  }
  cout << str << endl;
}
