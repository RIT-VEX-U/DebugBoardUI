#include "imgui.h"
#include <cstdio>
#include <format>
#include <string>
int main() {
  std::string s = std::format("Hello world");
  std::puts(s.c_str());
}
