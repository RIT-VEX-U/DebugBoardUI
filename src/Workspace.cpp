#include "Workspace.hpp"
#include "imgui.h"
#include "implot/implot.h"
#include "math.h"
#include "widgets/GraphUtils.hpp"
#include "widgets/WidgetRegistry.hpp"
#include <map>
namespace Workspace {
WidgetRegistry reg;

bool draw_plot_window;
void Demo_RealtimePlots();
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

  ImGui::BeginMainMenuBar();

  ImGui::EndMainMenuBar();

  for (auto [id, widget] : active_widgets) {
    widget->Draw();
  }

  if (draw_plot_window) {
    ImPlot::ShowDemoWindow(&draw_plot_window);
  }
}
float to_radian(float deg) { return (deg * (3.14 / 180.0)); }
static float knob_value = 0;

void Demo_RealtimePlots() {
  ImGui::BulletText("Move your mouse to change the data!");
  ImGui::BulletText(
      "This example assumes 60 FPS. Higher FPS requires larger buffer size.");
  static ScrollingBuffer sdata1, sdata2;
  static RollingBuffer rdata1, rdata2;
  ImVec2 mouse = ImGui::GetMousePos();
  static float t = 0;
  t += ImGui::GetIO().DeltaTime;
  sdata1.AddPoint(t, mouse.x * 0.0005f);
  rdata1.AddPoint(t, mouse.x * 0.0005f);
  sdata2.AddPoint(t, mouse.y * 0.0005f);
  rdata2.AddPoint(t, mouse.y * 0.0005f);

  static float history = 10.0f;
  ImGui::SliderFloat("History", &history, 1, 30, "%.1f s");
  rdata1.Span = history;
  rdata2.Span = history;

  static ImPlotAxisFlags flags =
      ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoTickMarks;

  if (ImPlot::BeginPlot("Scrolling", ImVec2(-1, 250))) {
    ImPlot::SetupAxes("Scrolling Title a", "Scrolling Title b", flags, flags);
    ImPlot::SetupAxisLimits(ImAxis_X1, t - history, t, ImGuiCond_Always);
    ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1);
    ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
    ImPlot::PlotShaded("Mouse X", &sdata1.Data[0].x, &sdata1.Data[0].y,
                       sdata1.Data.size(), -INFINITY, 0, sdata1.Offset,
                       2 * sizeof(float));
    ImPlot::PlotLine("Mouse Y", &sdata2.Data[0].x, &sdata2.Data[0].y,
                     sdata2.Data.size(), 0, sdata2.Offset, 2 * sizeof(float));
    ImPlot::EndPlot();
  }
  if (ImPlot::BeginPlot("Rolling", ImVec2(-1, 250))) {
    ImPlot::SetupAxes(NULL, NULL, flags, flags);
    ImPlot::SetupAxisLimits(ImAxis_X1, 0, history, ImGuiCond_Always);
    ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1);
    ImPlot::PlotLine("Mouse X", &rdata1.Data[0].x, &rdata1.Data[0].y,
                     rdata1.Data.size(), 0, 0, 2 * sizeof(float));
    ImPlot::PlotLine("Mouse Y", &rdata2.Data[0].x, &rdata2.Data[0].y,
                     rdata2.Data.size(), 0, 0, 2 * sizeof(float));
    ImPlot::EndPlot();
  }
}

} // namespace Workspace
