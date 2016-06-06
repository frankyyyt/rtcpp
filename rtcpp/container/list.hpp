#pragma once

#include <memory>

namespace rt
{

template <class T>
struct list_node {
  using pointer = list_node*;
  pointer next;
  T info;
  list_node() : next(nullptr), info() {}
  list_node(pointer p, const T& v) : next(p), info(v) {}
};

template <class T, class Allocator = std::allocator<T>>
class list {
  private:
  using node_type = list_node<T>;
  using node_pointer = node_type*;
  node_type head;
  public:
  list()
  {
    head.next = &head;
  }
  void push_front(const T& v)
  {
    node_pointer p = new node_type(head.next, v);
    head.next = p;
  }
  node_pointer begin() {return head.next;}
  node_pointer end() {return &head;}
};

}

