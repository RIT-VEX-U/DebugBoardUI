#pragma once

#include <vector>

// utility structure for realtime plot
template <typename ElementType> class ScrollingBuffer {
public:
  using value_type = ElementType;
  int MaxSize;
  int Offset;
  std::vector<value_type> Data;

  explicit ScrollingBuffer(int max_size = 2000) {
    MaxSize = max_size;
    Offset = 0;
    Data.reserve(MaxSize);
  }
  void AddPoint(value_type y) {
    if (Data.size() < MaxSize)
      Data.push_back(y);
    else {
      Data[Offset] = y;
      Offset = (Offset + 1) % MaxSize;
    }
  }
  void Erase() {
    if (Data.size() > 0) {
      Data.shrink(0);
      Offset = 0;
    }
  }
};

// utility structure for realtime plot
template <typename ElementType> struct RollingBuffer {
public:
  using value_type = ElementType;
  size_t MaxSize;
  ;
  std::vector<value_type> Data;

  explicit RollingBuffer(size_t max_size = 2000) {
    MaxSize = max_size;
    Data.reserve(MaxSize);
  }
  void AddPoint(value_type val) {
    if (Data.size() == MaxSize) {
      Data.resize(0);
    }
    Data.push_back(val);
  }
};
