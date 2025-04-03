#include "Workspace.hpp"
#include "Types.hpp"
#include "Widget.hpp"
#include "imgui.h"
#include "implot/implot.h"
#include "widgets/WidgetRegistry.hpp"
#include <algorithm>
#include <cmath>
#include <memory>
#include <print>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
namespace Workspace {
static WidgetRegistry reg;
static std::vector<std::shared_ptr<DataSource>> sources = {};

void AddSource(const std::shared_ptr<DataSource> &src) {
  sources.push_back(src);
}

static bool draw_plot_window = false;
static bool draw_demo_window = false;

static std::unordered_map<DataLocator, DataAndTime, DataLocatorHasher>
    seen_data{};

static std::unordered_map<WidgetId, Widget> active_widgets = {};
static WidgetId widget_id_count = 0;
static WidgetId NextWidgetId() {
  widget_id_count++;
  return widget_id_count;
}

std::unordered_set<WidgetId> activeWidgetsThatNeedThisData(DataLocator loc) {
  std::unordered_set<WidgetId> needies{};
  for (const auto &[id, widget] : active_widgets) {
    if (widget->WantedData().contains(loc)) {
      needies.emplace(id);
    }
  }

  return needies;
}

void RouteData() {
  std::unordered_set<WidgetId> to_update{};

  Timestamp rx_time = std::chrono::steady_clock::now();
  // foreach source
  for (const auto &source : sources) {
    const std::vector<DataUpdate> &src_updates = source->PollData();
    // foreach new update
    for (const DataUpdate &up : src_updates) {
      // foreach piece of data in that update
      for (const DataElement &data : up.new_data) {
        seen_data[data.location] = DataAndTime{data.value, up.rx_time};

        auto more = activeWidgetsThatNeedThisData(data.location);
        to_update.merge(more);
      }
    }
  }
  for (WidgetId id : to_update) {
    if (!active_widgets.contains(id)) {
      continue;
    }
    TimedData packet{};
    Widget &widg = active_widgets.at(id);
    for (const DataLocator &loc : widg->WantedData()) {
      if (loc.isEmpty() || !seen_data.contains(loc)) {
        std::println("REGISTERED UPDATES FROM EMPTY CHANNEL or CHANNEL THAT "
                     "DOESNT EXIST\n");
        continue;
      }
      packet[loc] = seen_data.at(loc);
    }
    widg->ReceiveData(packet);
  }

  // for (const auto& [key, widget] : active_widgets) {
  //     for (const DataLocator &wanted : widget->WantedData()) {
  //         for (const std::pair<std::string, DataUpdate> &name_and_up :
  //         updates) {
  //             const DataUpdate &update = name_and_up.second;
  //             for (const auto &specific_data : update.new_data) {
  //                 if (specific_data.location == wanted) {
  //                     widget->ReceiveData(specific_data);
  //                 }
  //             }
  //         }
  //     }
  // }
}

void Init() { AddWidgetsToCollection(reg); }

static void OpenWidget(const DefaultWidgetCreator &widg_maker) {
  WidgetId const nextId = NextWidgetId();
  Widget const widg = widg_maker(nextId);

  active_widgets[nextId] = widg;
}
static void CloseWidget(const Widget &widg) {
  WidgetId const id = widg->Id();
  active_widgets.erase(id);
}

static void DrawNewWidgetUI() {
  static ImGuiTextFilter filter;

  ImGui::Begin("New Widget");
  filter.Draw();
  for (const auto &[key, val] : reg) {
    if (filter.PassFilter(key.c_str())) {
      if (ImGui::Button(key.c_str())) {
        OpenWidget(val);
      }
    }
  }
  ImGui::End();
}
static void DrawMenuBar() {
  ImGui::BeginMainMenuBar();
  {
    {
      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Open Workspace", "Ctrl + O")) {
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

static void DrawNewDatasourceUI() {
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
  DrawMenuBar();

  DrawNewWidgetUI();
  DrawNewDatasourceUI();
  for (const auto &source : sources) {
    source->Draw();
  }

  ImGui::EndMainMenuBar();

  std::vector<WidgetId> to_close{};
  for (const auto &[id, widget] : active_widgets) {
    bool open = true;
    widget->Draw(&open);
    if (!open) {
      to_close.push_back(widget->Id());
    }
  }
  for (WidgetId const id : to_close) {
    CloseWidget(active_widgets[id]);
  }

  if (draw_plot_window) {
    ImPlot::ShowDemoWindow(&draw_plot_window);
  }
  if (draw_demo_window) {
    ImGui::ShowDemoWindow(&draw_demo_window);
  }
}

} // namespace Workspace

static bool DataPathMenu(DataLocator &current, const std::string &source_name,
                         const std::shared_ptr<DataSource> &source) {
  DataSource::DataElementSet const chans = source->ProvidedData();

  if (ImGui::MenuItem("receive time")) {
    current.source_name = source_name;
    current.path = {{"recv_time"}};
    current.is_rx_time = true;
  }
  ImGui::Separator();

  for (const DataElementDescription &chan : chans) {
    auto str = chan.path.toString();
    bool youSelected =
        current.path == chan.path && current.source_name == source_name;
    if (ImGui::MenuItem(str.c_str(), nullptr, &youSelected)) {
      current.path = chan.path;
      current.source_name = source_name;
      current.is_rx_time = false;
      return true;
    }
  }
  return false;
}

bool DataLocationSelector(const char *name, DataLocator &current) {
  std::string const current_name = current.toString();
  bool changed = false;

  float const winwidth = ImGui::GetWindowWidth();
  ImGui::TextUnformatted(name);
  float avail_width = winwidth - ImGui::GetCursorPosX();
  float const textwidth = ImGui::CalcTextSize(current_name.c_str()).x;
  avail_width = std::min(textwidth, avail_width);
  ImGui::SameLine();
  std::string const child_name = std::string("## source selector ") + name;
  if (ImGui::BeginChild(
          child_name.c_str(),
          ImVec2(avail_width, ImGui::GetTextLineHeightWithSpacing()))) {
    if (ImGui::BeginMenu(current_name.c_str())) {
      for (const std::shared_ptr<DataSource> &src : Workspace::sources) {
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
