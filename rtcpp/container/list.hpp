#pragma once

namespace rt
{

struct list_node {
  using pointer = list_node*;
  pointer next;
  int info;
  list_node() : next(nullptr), info() {}
  list_node(pointer p, const int& v) : next(p), info(v) {}
};

class list {
  private:
  using node_type = list_node;
  using node_pointer = node_type*;
  node_type head;
  public:
  list()
  {
    head.next = &head;
  }
  void push_front(const int& v)
  {
    node_pointer p = new node_type(head.next, v);
    head.next = p;
  }
  node_pointer begin() {return head.next;}
  node_pointer end() {return &head;}
};

}

