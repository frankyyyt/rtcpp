#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <functional>

#include <rtcpp/container/list.hpp>

int main()
{
  using namespace rt;

  list<int> l;
  l.push_front(4);
  l.push_front(3);
  l.push_front(2);
  l.push_front(1);
  l.push_back(5);
  l.push_back(6);
  l.push_back(7);
  l.push_back(8);
  l.print();
  std::cout << "________" << std::endl;
  l.reverse();
  l.print();
  std::cout << "________" << std::endl;
  //l.sort();
  l.print();
  std::cout << "________" << std::endl;

  return 0;
}

