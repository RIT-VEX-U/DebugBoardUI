#include "imgui-knobs/imgui-knobs.h"
#include "imgui.h"
#include "implot/implot.h"

#include "platform/glue.hpp"
#include <math.h>

#include "widgets/ControlSystems/PidVizWidget.hpp"
#include "widgets/Widget.hpp"
#include <cstdio>
#include <format>
#include <string>
float to_radian(float deg) { return (deg * (3.14 / 180.0)); }
static float knob_value = 0;

// utility structure for realtime plot
struct ScrollingBuffer {
  int MaxSize;
  int Offset;
  ImVector<ImVec2> Data;
  ScrollingBuffer(int max_size = 2000) {
    MaxSize = max_size;
    Offset = 0;
    Data.reserve(MaxSize);
  }
  void AddPoint(float x, float y) {
    if (Data.size() < MaxSize)
      Data.push_back(ImVec2(x, y));
    else {
      Data[Offset] = ImVec2(x, y);
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
struct RollingBuffer {
  float Span;
  ImVector<ImVec2> Data;
  RollingBuffer() {
    Span = 10.0f;
    Data.reserve(2000);
  }
  void AddPoint(float x, float y) {
    float xmod = fmodf(x, Span);
    if (!Data.empty() && xmod < Data.back().x)
      Data.shrink(0);
    Data.push_back(ImVec2(xmod, y));
  }
};

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

int main() {
  InitialRegisterWidgets();

  Platform::Data pdata = Platform::init("Debug Board UI");

  // Our state
  bool show_demo_window = true;
  bool show_plot_window = true;

  // Main loop
  while (!Platform::shouldclose(pdata)) {
    Platform::prerender(pdata);

    if (show_demo_window) {
      ImGui::ShowDemoWindow(&show_demo_window);
    }
    if (show_plot_window) {
      ImPlot::ShowDemoWindow(&show_demo_window);
    }

    DrawNewWidgetUI();

    Platform::postrender(pdata);
  }

  Platform::cleanup(pdata);
}