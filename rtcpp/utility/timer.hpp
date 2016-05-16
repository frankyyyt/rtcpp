#pragma once

#include <chrono>

namespace rt {

class timer {
  private:
  std::chrono::time_point<std::chrono::system_clock> m_start;
  public:
  timer()
  : m_start(std::chrono::system_clock::now())
  {}
  ~timer()
  {
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    auto diff = end - m_start;
    auto diff2 = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
    std::cout << diff2.count() << " ";
  }
};

}

