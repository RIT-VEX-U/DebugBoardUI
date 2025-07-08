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
#include "datasources/DebugBoard.hpp"

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
/**
 * @return a set of widgets that need the data at the data locator
 * @param loc the data locator for the data that the widgets need
 */
std::unordered_set<WidgetId> activeWidgetsThatNeedThisData(DataLocator loc) {
  //the set of widgets that need the data
  std::unordered_set<WidgetId> needies{};
  //for each id and widget in our active widgets
  for (const auto &[id, widget] : active_widgets) {
    //check if the widget wants the data we have
    if (widget->WantedData().contains(loc)) {
      //put the widget into our set
      needies.emplace(id);
    }
  }

  return needies;
}
/**
 * Routes data to where it needs to be
 */
void RouteData() {
  //widget we want to update with the data 
  std::unordered_set<WidgetId> to_update{};

  //the time that the data was recieved at
  Timestamp rx_time = std::chrono::steady_clock::now();

  // foreach data source
  for (const auto &source : sources) {
    //poll the data at said source
    const std::vector<DataUpdate> &src_updates = source->PollData();
    // foreach new update
    for (const DataUpdate &up : src_updates) {
      // foreach piece of data in that update
      for (const DataElement &data : up.new_data) {
        //get said data's value and the time when it was received
        seen_data[data.location] = DataAndTime{data.value, up.rx_time};
        //get the widgets that need said data
        auto more = activeWidgetsThatNeedThisData(data.location);
        //merge those widgets with our list of widgets we need to update
        to_update.merge(more);
      }
    }
  }
  //foreach widget id in our list of widgets to update
  for (WidgetId id : to_update) {
    //check if the widget is real
    if (!active_widgets.contains(id)) {
      continue;
    }
    //the packet of data we need to send to the widget
    TimedData packet{};
    //get the active widget at the id
    Widget &widg = active_widgets.at(id);
    //foreach data locator in our widget's wanted data
    for (const DataLocator &loc : widg->WantedData()) {
      // std::println("I want: {}", loc.toString());
      //check if the data is real and we have seen it
      if (loc.isEmpty() || !seen_data.contains(loc)) {
        std::println("REGISTERED UPDATES FROM EMPTY CHANNEL or CHANNEL THAT "
                     "DOESNT EXIST\n");
        continue;
      }
      //get the data that we need from what we have seen and put it into the packet
      packet[loc] = seen_data.at(loc);
    }
    //send the data we got from the websocket to the widget
    widg->ReceiveData(packet);
  }
  
  //foreach widget id in all of our active widgets
  for(auto [id, widget]: active_widgets){
    //if the widget has data to send 
    if(!widget->DataToSend().loc.isEmpty()){
      for(const auto &source: sources){
        source.get()->SendData(widget->DataToSend());
      }
      widget->ClearSentData();
    }
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
/**
 * Basic Drawing of our widgets on the gui
 */
void Draw() {
  //draws specific startup widgets
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
