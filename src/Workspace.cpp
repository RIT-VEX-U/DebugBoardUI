#include "Workspace.hpp"
#include "imgui.h"
#include "widgets/WidgetRegistry.hpp"
#include <map>
namespace Workspace {
WidgetRegistry reg;

void Init() { AddWidgetsToCollection(reg); }

static std::unordered_map<WidgetId, Widget> active_widgets = {};
static WidgetId widget_id_count = 0;
WidgetId NextWidgetId() {
  widget_id_count++;
  return widget_id_count;
}

void OpenWidget(DefaultWidgetCreator widg_maker) {
  WidgetId nextId = NextWidgetId();
  Widget widg = widg_maker(nextId);

  active_widgets[nextId] = widg;
}
void CloseWidget(Widget widg) {
  WidgetId id = widg->Id();
  active_widgets.erase(id);
}

void DrawNewWidgetUI() {
  static ImGuiTextFilter filter;

  ImGui::Begin("New Widget");
  filter.Draw();
  for (auto [key, val] : reg) {
    if (filter.PassFilter(key.c_str())) {
      if (ImGui::Button(key.c_str())) {
        OpenWidget(val);
      }
    }
  }
  ImGui::End();
}
void Draw() {
  auto vp = ImGui::GetMainViewport();
  ImGuiID id = 0;
  ImGuiID dockspace_id = ImGui::DockSpaceOverViewport(id, vp);
  DrawNewWidgetUI();

  for (auto [id, widget] : active_widgets) {
    widget->Draw();
  }
}
} // namespace Workspace
