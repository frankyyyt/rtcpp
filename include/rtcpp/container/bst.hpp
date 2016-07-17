#include <memory>

namespace rt { namespace bst {

template <class T, class Ptr>
struct node {
  using value_type = T;
  using self_pointer = typename std::pointer_traits<Ptr>::template
    rebind<node<T, Ptr>>;
  self_pointer link[2];
  value_type key;
};

}
}

