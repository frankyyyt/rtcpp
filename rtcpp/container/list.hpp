#pragma once

#include <memory>
#include <utility>

namespace rt
{

template <class T>
struct list_node {
  using pointer = list_node*;
  pointer next;
  pointer prev;
  T info;
  list_node() : next(nullptr), prev(nullptr), info() {}
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
    head.prev = &head;
  }
  node_pointer get_node() {return new node_type;}
  void insert(node_pointer pos, node_pointer p, const T& value)
  {
    p->prev = pos->prev;
    pos->prev = p;
    p->next = pos;
    p->prev->next = p;
  }
  void push_front(const T& v)
  {
    auto p = get_node();
    p->info = v;
    insert(head.next, p, v);
  }
  void push_back(const T& v)
  {
    auto p = get_node();
    p->info = v;
    insert(&head, p, v);
  }
  void print() const
  {
    node_pointer p = head.next;
    while (p != &head) {
      std::cout << p->info << std::endl;
      p = p->next;
    }
  }
  void reverse()
  {
    node_pointer p = &head;
    do {
      std::swap(p->next, p->prev);
      p = p->prev;
    } while (p != &head);
  }
  //void swap_nodes(node_pointer p, node_pointer q)
  //{
  //  p->next->prev = q;
  //  q->next->prev = p;

  //  p->prev->next = q;
  //  q->prev->next = p;

  //  std::swap(p->prev, q->prev);
  //  std::swap(p->next, q->next);
  //}
  //void sort()
  //{
  //  node_pointer p = head.next->next;
  //  while (p != &head) {
  //    node_pointer q = head.next;
  //    while (q != p) {
  //      if (p->info < q->info) {
  //        swap_nodes(p, q);
  //        p = q->next;
  //        break;
  //      }
  //      q = q->next;
  //    }
  //    p = p->next;
  //  }
  //}
  node_pointer begin() {return head.next;}
  node_pointer end() {return &head;}
};

}

