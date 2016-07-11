#include <list>
#include <array>
#include <stack>
#include <deque>
#include <vector>
#include <iostream>

#include <rtcpp/utility/timer.hpp>
#include <rtcpp/utility/to_number.hpp>

int main(int argc, char* argv[])
{
  if ((argc != 3)) {
    std::cout <<
    "\nUsage: $ ./bench_stack N S\n"
    "N: Number of elements in the stack.\n"
    "S: Repeat.\n"
    << std::endl;
    std::cout <<
    "The program output has the following layout: \n"
    "(0) (1) (2) (3) (4) (5): \n"
    "Where: \n"
    "(0)  Number of elements.\n"
    "(1)  std::stack<T, std::list<T>>\n"
    "(2)  std::stack<T, std::deque<T>>\n"
    "(3)  std::stack<T, std::vector<T>>\n"
    "(4)  std::vector<T> with reserve.\n"
    << std::endl;

    return 0;
  }

  using namespace rt;

  const int N = to_number<int>(argv[1]);
  const int S = to_number<int>(argv[2]);

  using value_type = std::array<char, 512>;
  //using value_type = int;
  value_type v;

  const int step = 10000;
  std::cout << "std::stack<T, std::list<T>>" << std::endl;
  for (int i = 0; i < S; ++i) {
    std::stack<value_type, std::list<value_type>> s;

    const int n = N + i * step;
    std::cout << n << " ";
    {
      rt::timer t;
      for (int j = 0; j < n; ++j)
        s.push(v);

      for (int j = 0; j < n; ++j)
        s.pop();
    }
    std::cout << "\n";
  }

  std::cout << std::endl;
  std::cout << "std::stack<T, std::deque<T>>" << std::endl;
  for (int i = 0; i < S; ++i) {
    std::stack<value_type, std::deque<value_type>> s;

    const int n = N + i * step;
    std::cout << n << " ";
    {
      rt::timer t;
      for (int j = 0; j < n; ++j)
        s.push(v);

      for (int j = 0; j < n; ++j)
        s.pop();
    }
    std::cout << "\n";
  }

  std::cout << std::endl;
  std::cout << "std::stack<T, std::vector<T>>" << std::endl;
  for (int i = 0; i < S; ++i) {
    std::stack<value_type, std::vector<value_type>> s;

    const int n = N + i * step;
    std::cout << n << " ";
    {
      rt::timer t;
      for (int j = 0; j < n; ++j)
        s.push(v);

      for (int j = 0; j < n; ++j)
        s.pop();
    }
    std::cout << "\n";
  }

  std::cout << std::endl;
  std::cout << "std::vector<T, std::vector<T>> with reserve." << std::endl;
  for (int i = 0; i < S; ++i) {
    const int n = N + i * step;
    std::vector<value_type> vec;
    vec.reserve(n);
    std::cout << n << " ";
    {
      rt::timer t;
      for (int j = 0; j < n; ++j)
        vec.push_back(v);

      for (int j = 0; j < n; ++j)
        vec.pop_back();
    }
    std::cout << "\n";
    vec.resize(0);
  }
  std::cout << std::endl;

  return 0;
}

