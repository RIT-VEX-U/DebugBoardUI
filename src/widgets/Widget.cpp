#include "Widget.hpp"
#include "imgui.h"
#include <format>
#include <map>

static std::unordered_map<std::string, DefaultWidgetCreator> registered_widgets;

void RegisterWidget(std::string name, DefaultWidgetCreator newDefault) {
  registered_widgets[name] = newDefault;
}

void DrawNewWidgetUI() {
  static ImGuiTextFilter filter;

  ImGui::Begin("New Widget");
  filter.Draw();
  for (auto [key, val] : registered_widgets) {
    if (filter.PassFilter(key.c_str())) {
      if (ImGui::Button(key.c_str())) {
        std::puts(std::format("New Widget {}", key).c_str());
      }
    }
  }
  ImGui::End();
}