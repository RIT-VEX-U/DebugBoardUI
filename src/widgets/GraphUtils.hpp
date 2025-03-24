#pragma once
#include "imgui.h"

// utility structure for realtime plot
class ScrollingBuffer {
public:
  int MaxSize;
  int Offset;
  ImVector<ImVec2> Data;

  ScrollingBuffer(int max_size = 2000);
  void AddPoint(float x, float y);
  void Erase();
};

// utility structure for realtime plot
struct RollingBuffer {
public:
  float Span;
  ImVector<ImVec2> Data;

  RollingBuffer();
  void AddPoint(float x, float y);
};
