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
  list();
  node_pointer get_node() {return new node_type;}
  void insert(node_pointer pos, node_pointer p, const T& value);
  void push_front(const T& v);
  void push_back(const T& v);
  void print() const;
  void reverse();
  void move_behind(node_pointer p, node_pointer q);
  void swap_nodes(node_pointer p, node_pointer q);
  void sort();
  node_pointer begin() {return head.next;}
  node_pointer end() {return &head;}
};

template <class T, class A>
list<T, A>::list()
{
  head.next = &head;
  head.prev = &head;
}

template <class T, class A>
void list<T, A>::insert(node_pointer pos, node_pointer p,
  const T& value)
{
  p->prev = pos->prev;
  pos->prev = p;
  p->next = pos;
  p->prev->next = p;
}
template <class T, class A>
void list<T, A>::push_front(const T& v)
{
  auto p = get_node();
  p->info = v;
  insert(head.next, p, v);
}

template <class T, class A>
void list<T, A>::push_back(const T& v)
{
  auto p = get_node();
  p->info = v;
  insert(&head, p, v);
}

template <class T, class A>
void list<T, A>::reverse()
{
  node_pointer p = &head;
  do {
    std::swap(p->next, p->prev);
    p = p->prev;
  } while (p != &head);
}

template <class T, class A>
void list<T, A>::print() const
{
  node_pointer p = head.next;
  while (p != &head) {
    std::cout << p->info << std::endl;
    p = p->next;
  }
}

template <class T, class A>
void list<T, A>::swap_nodes(node_pointer q, node_pointer p)
{

  if (q->next == p) {
    p->next->prev = q;
    q->prev->next = p;
    p->prev = q->prev;
    q->next = p->next;
    p->next = q;
    q->prev = p;
    return;
  }

  p->next->prev = q;
  q->prev->next = p;

  p->prev->next = q;
  q->next->prev = p;

  std::swap(p->prev, q->prev);
  std::swap(p->next, q->next);
}

template <class T, class A>
void list<T, A>::move_behind(node_pointer q, node_pointer p)
{
  q->prev->next = p;
  p->next->prev = p->prev;
  p->prev->next = p->next;
  p->next = q;
  p->prev = q->prev;
  q->prev = p;
}

template <class T, class A>
void list<T, A>::sort()
{
  node_pointer p = head.next->next;
  while (p != &head) {
    node_pointer q = head.next;
    while (q != p) {
      if (p->info < q->info) {
        move_behind(q, p);
        p = q;
        break;
      }
      q = q->next;
    }
    p = p->next;
  }
}

}

