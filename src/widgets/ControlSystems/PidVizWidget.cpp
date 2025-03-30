#include "ControlSystems/PidVizWidget.hpp"
#include "Widget.hpp"
#include "imgui.h"
#include "implot/implot.h"
#include <../Types.hpp>
#include <cmath>
#include <print>

PidVizWidget::PidVizWidget(WidgetId id)
    : WidgetImpl(id), plot_error_(false), t_(0) {}

void PidVizWidget::ReceiveData(DataElement data) {
  if (data.path == sp_loc_) {
    t_ += 0.01;
    double const sp = std::get<double>(data.value);
    sdata_.AddPoint(ImVec2{(float)t_, (float)sp});
  }
}
void PidVizWidget::Draw(bool *should_close) {
    ImPlotAxisFlags const flags = 0;
    // ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoTickMarks;

    if (ImGui::Begin("Pid Visualizer", should_close)) {
        bool modified = false;
        modified |= DataLocationSelector("time", t_loc_);

        modified |= DataLocationSelector("sp", sp_loc_);
        modified |= DataLocationSelector("pv", pv_loc_);
        bool const any_bad = t_loc_.isEmpty() || sp_loc_.isEmpty() || pv_loc_.isEmpty();
        if (modified && !any_bad) {
            std::println("Registering new");
            RegisterDataCallback({t_loc_, sp_loc_, pv_loc_});
        }
        if (any_bad) {
            ImGui::PushStyleColor(ImGuiCol_Text, {1, 0, 0, 1});
            ImGui::Text("Some invalid data sources");
            ImGui::PopStyleColor();
        }

        ImGui::Checkbox("Plot error", &plot_error_);
        if (ImPlot::BeginPlot("Scrolling", ImVec2(-1, 250))) {
            float const history = 10.0;
            ImPlot::SetupAxes("Scrolling Title a", "Scrolling Title b", flags, flags);
            ImPlot::SetupAxisLimits(ImAxis_X1, t_ - history, t_, ImGuiCond_Always);
            ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1);
            ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5F);
            if (!sdata_.Data.empty()) {
                ImPlot::PlotLine("Data",
                                 &sdata_.Data[0].x,
                                 &sdata_.Data[0].y,
                                 (int) sdata_.Data.size(),
                                 0,
                                 sdata_.Offset,
                                 2 * sizeof(float));
            }
            ImPlot::EndPlot();
        }
  }

  ImGui::End();
}