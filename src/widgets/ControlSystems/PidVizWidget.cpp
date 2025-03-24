#include "ControlSystems/PidVizWidget.hpp"
#include "imgui.h"
#include "implot/implot.h"
#include "math.h"

PidVizWidget::PidVizWidget(WidgetId id) : WidgetImpl(id), plot_error_(false) {
  sdata.AddPoint(t, 0);
}
PidVizWidget::~PidVizWidget() {}
// void PidVizWidget::RegisterDataCallback() {}

void PidVizWidget::ReceiveData() {}
void PidVizWidget::Draw() {
  t += 0.01;
  ImPlotAxisFlags flags =
      ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoTickMarks;

  sdata.AddPoint(t, sin(t));
  if (ImGui::Begin("Pid Visualizer")) {
    ImGui::Checkbox("Plot error", &plot_error_);
    float history = 10.0;
    if (ImPlot::BeginPlot("Scrolling", ImVec2(-1, 250))) {
      ImPlot::SetupAxes("Scrolling Title a", "Scrolling Title b", flags, flags);
      ImPlot::SetupAxisLimits(ImAxis_X1, t - history, t, ImGuiCond_Always);
      ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1);
      ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
      ImPlot::PlotLine("Data", &sdata.Data[0].x, &sdata.Data[0].y,
                       sdata.Data.size(), 0, sdata.Offset, 2 * sizeof(float));

      ImPlot::EndPlot();
    }
  }

  ImGui::End();
}