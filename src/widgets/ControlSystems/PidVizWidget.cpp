#include "ControlSystems/PidVizWidget.hpp"
#include "imgui.h"
#include "implot/implot.h"
#include "math.h"
#include <../Types.hpp>

PidVizWidget::PidVizWidget(WidgetId id)
    : WidgetImpl(id), plot_error_(false), t(0) {}
PidVizWidget::~PidVizWidget() {}
// void PidVizWidget::RegisterDataCallback() {}

void PidVizWidget::ReceiveData(DataElement data) {
  // printf("%s ?= %s\n", data.path.toString().c_str(),
  // sp_loc.toString().c_str());
  if (data.path == sp_loc) {
    t += 0.01;
    double sp = std::get<double>(data.value);
    // printf("I got %s: %f\n", data.path.toString().c_str(), sp);
    sdata.AddPoint(t, sp);
  }
}
void PidVizWidget::Draw(bool *should_close) {

  ImPlotAxisFlags flags = 0;
  // ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoTickMarks;

  if (ImGui::Begin("Pid Visualizer", should_close)) {
    bool modified = false;
    modified |= DataLocationSelector("time", t_loc);

    modified |= DataLocationSelector("sp", sp_loc);
    modified |= DataLocationSelector("pv", pv_loc);
    bool any_bad = t_loc.isEmpty() || sp_loc.isEmpty() || pv_loc.isEmpty();
    if (modified && !any_bad) {
      printf("Registering new\n");
      RegisterDataCallback({t_loc, sp_loc, pv_loc});
    }
    if (any_bad) {
      ImGui::PushStyleColor(ImGuiCol_Text, {1, 0, 0, 1});
      ImGui::Text("Some invalid data sources");
      ImGui::PopStyleColor();
    }

    ImGui::Checkbox("Plot error", &plot_error_);
    float history = 10.0;
    if (ImPlot::BeginPlot("Scrolling", ImVec2(-1, 250))) {
      ImPlot::SetupAxes("Scrolling Title a", "Scrolling Title b", flags, flags);
      ImPlot::SetupAxisLimits(ImAxis_X1, t - history, t, ImGuiCond_Always);
      ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1);
      ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
      if (sdata.Data.size() > 0) {

        ImPlot::PlotLine("Data", &sdata.Data[0].x, &sdata.Data[0].y,
                         sdata.Data.size(), 0, sdata.Offset, 2 * sizeof(float));
      }
      ImPlot::EndPlot();
    }
  }

  ImGui::End();
}