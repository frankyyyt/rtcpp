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
  l.push_front(1);
  l.push_front(2);
  l.push_front(3);
  l.push_front(4);
  auto p = l.begin();
  auto end = l.end();
  while (p != end) {
    std::cout << p->info << std::endl;
    p = p->next;
  }

  return 0;
}

