#include "GraphUtils.hpp"
#include "math.h"

ScrollingBuffer::ScrollingBuffer(int max_size) {
  MaxSize = max_size;
  Offset = 0;
  Data.reserve(MaxSize);
}
void ScrollingBuffer::AddPoint(float x, float y) {
  if (Data.size() < MaxSize)
    Data.push_back(ImVec2(x, y));
  else {
    Data[Offset] = ImVec2(x, y);
    Offset = (Offset + 1) % MaxSize;
  }
}
void ScrollingBuffer::Erase() {
  if (Data.size() > 0) {
    Data.shrink(0);
    Offset = 0;
  }
}

RollingBuffer::RollingBuffer() {
  Span = 10.0f;
  Data.reserve(2000);
}
void RollingBuffer::AddPoint(float x, float y) {
  float xmod = fmodf(x, Span);
  if (!Data.empty() && xmod < Data.back().x)
    Data.shrink(0);
  Data.push_back(ImVec2(xmod, y));
}
