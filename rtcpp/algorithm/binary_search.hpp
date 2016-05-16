
namespace rt
{

template<class RandomAccessIter, class T>
bool binary_search(RandomAccessIter begin, RandomAccessIter end, const T& K)
{
  if (begin == end)
    return true;

  const auto N = std::distance(begin, end);
  int l = 1;
  int u = N;

  for (;;) {
    if (u < l)
      return false;

    int i = (l + u) >> 1;

    if (K < begin[i - 1]) {
      u = i - 1;
      continue;
    }

    if (begin[i - 1] < K) {
      l = i + 1;
      continue;
    }

    return true;
  }
}

}

