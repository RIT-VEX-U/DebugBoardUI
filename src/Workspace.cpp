#include "Workspace.hpp"
#include "Types.hpp"
#include "imgui.h"
#include "implot/implot.h"
#include "math.h"
#include "widgets/GraphUtils.hpp"
#include "widgets/WidgetRegistry.hpp"
#include <map>
namespace Workspace {
WidgetRegistry reg;
std::vector<std::shared_ptr<DataSource>> sources = {};

void AddSource(std::shared_ptr<DataSource> src) { sources.push_back(src); }

bool draw_plot_window = false;
bool draw_demo_window = false;

static std::unordered_map<WidgetId, Widget> active_widgets = {};
static WidgetId widget_id_count = 0;
WidgetId NextWidgetId() {
  widget_id_count++;
  return widget_id_count;
}

void RouteData() {
  std::vector<std::pair<std::string, DataUpdate>> updates{};
  for (auto source : sources) {
    std::vector<DataUpdate> src_updates = source->PollData();
    for (auto up : src_updates) {
      updates.push_back({source->Name(), up});
    }
  }
  for (auto [key, widget] : active_widgets) {
    for (DataLocator wanted : widget->WantedData()) {
      for (auto up : updates) {
        std::string src = up.first;
        const DataUpdate &update = up.second;
        for (auto specific_data : update.new_data) {
          if (specific_data.path == wanted) {
            widget->ReceiveData(specific_data);
          }
        }
      }
    }
  }
}

void Init() { AddWidgetsToCollection(reg); }

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
void DrawMenuBar() {
  ImGui::BeginMainMenuBar();
  {
    {
      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Open Workspace", "Ctrl + O")) {
          printf("Open workspace\n");
        }
        ImGui::MenuItem("Save Workspace", "Ctrl + S");
        ImGui::EndMenu();
      }
    }

    {
      if (ImGui::BeginMenu("Help")) {
        ImGui::Checkbox("Show ImGui Demo", &draw_demo_window);
        ImGui::Checkbox("Show ImPlot Demo", &draw_plot_window);
        ImGui::MenuItem("Information about this");
        ImGui::EndMenu();
      }
    }
  }
}

void DrawNewDatasourceUI() {
  if (ImGui::Begin("Datasources")) {
    if (ImGui::CollapsingHeader("New Source", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Button("Debug Board");
      ImGui::Button("File");
      ImGui::Button("Programming");
    }
    ImGui::Separator();
  }
  ImGui::End();
}

void Draw() {
  auto vp = ImGui::GetMainViewport();
  ImGuiID id = 0;
  ImGuiID dockspace_id = ImGui::DockSpaceOverViewport(id, vp);
  (void)dockspace_id;
  DrawMenuBar();

  DrawNewWidgetUI();
  DrawNewDatasourceUI();
  for (auto source : sources) {
    source->Draw();
  }

  ImGui::EndMainMenuBar();

  std::vector<WidgetId> to_close{};
  for (auto [id, widget] : active_widgets) {
    bool open = true;
    widget->Draw(&open);
    if (!open) {
      to_close.push_back(widget->Id());
    }
  }
  for (WidgetId id : to_close) {
    CloseWidget(active_widgets[id]);
  }

  if (draw_plot_window) {
    ImPlot::ShowDemoWindow(&draw_plot_window);
  }
  if (draw_demo_window) {
    ImGui::ShowDemoWindow(&draw_demo_window);
  }
}
float to_radian(float deg) { return (deg * (3.14 / 180.0)); }
static float knob_value = 0;

} // namespace Workspace

bool DataPathMenu(DataLocator &current, std::string source_name,
                  std::shared_ptr<DataSource> source) {
  DataSource::DataElementSet chans = source->ProvidedData();

  if (ImGui::MenuItem("receive time")) {
    current.source_name = source_name;
    current.path = {{"recv_time"}};
    current.special = true;
  }
  ImGui::Separator();

  for (DataElementDescription chan : chans) {
    auto str = chan.path.toString();
    bool youSelected =
        current.path == chan.path && current.source_name == source_name;
    if (ImGui::MenuItem(str.c_str(), nullptr, &youSelected)) {
      current.path = chan.path;
      current.source_name = source_name;
      current.special = false;
      return true;
    }
  }
  return false;
}

bool DataLocationSelector(const char *name, DataLocator &current) {

  std::string current_name = current.toString();
  bool changed = false;

  float winwidth = ImGui::GetWindowWidth();
  ImGui::TextUnformatted(name);
  float avail_width = winwidth - ImGui::GetCursorPosX();
  float textwidth = ImGui::CalcTextSize(current_name.c_str()).x;
  if (textwidth < avail_width) {
    avail_width = textwidth;
  }
  ImGui::SameLine();
  std::string child_name = std::string("## source selector ") + name;
  if (ImGui::BeginChild(
          child_name.c_str(),
          ImVec2(avail_width, ImGui::GetTextLineHeightWithSpacing()))) {
    if (ImGui::BeginMenu(current_name.c_str())) {
      for (std::shared_ptr<DataSource> src : Workspace::sources) {
        auto srcname = src->Name();
        if (ImGui::BeginMenu(srcname.c_str())) {
          if (DataPathMenu(current, srcname, src)) {
            changed = true;
          }
          ImGui::EndMenu();
        }
      }
      ImGui::EndMenu();
    }
  }
  ImGui::EndChild();

  return changed;
}