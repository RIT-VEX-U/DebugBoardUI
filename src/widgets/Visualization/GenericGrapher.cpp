#include "Visualization/GenericGrapher.hpp"
#include "imgui.h"
#include "implot/implot.h"

GenericGrapher::GenericGrapher(WidgetId id) : WidgetImpl(id) {}

void GenericGrapher::ReceiveData(DataElement el) {}
void GenericGrapher::Draw(bool *should_close) {
  std::string dataname = std::format("Generic Drawer Inputs #{}", Id());
  std::string plotname = std::format("Generic Drawer Plot #{}", Id());
  if (ImGui::Begin(dataname.c_str()), should_close) {

    DataLocationSelector("Time", time_loc);

    ImGui::Separator();
    bool reregister = false;
    for (size_t i = 0; i < data.size(); i++) {
      std::string name = std::format("Series {}", i);
      reregister |= DataLocationSelector(name.c_str(), data[i].loc);
    }
    if (ImGui::Button("Add Series")) {
      data.push_back(AxisData{
          .secondary_y = false,
          .loc = DataLocator{},
          .data = {},
      });
    }
  }
  ImGui::End();

  if (ImGui::Begin(plotname.c_str())) {
    ImPlotAxisFlags flags = 0;
    // ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoTickMarks;

    if (ImPlot::BeginPlot("Scrolling", ImVec2(-1, 250))) {
      ImPlot::SetupAxes("Scrolling Title a", "Scrolling Title b", flags, flags);
      auto t = std::chrono::duration<double>{most_recent_time};

      ImPlot::SetupAxisLimits(ImAxis_X1, t.count() - 10.0, t.count(),
                              ImGuiCond_Always);
      ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1);
      ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
      for (int i = 0; i < data.size(); i++) {
        const AxisData &mydata = data[i];
        if (mydata.data.Data.size() == 0 || data[i].loc.isEmpty()) {
          // skip invalid guys
          continue;
        }

        ImPlot::PlotLine("Data", &time.Data[0], &mydata.data.Data[0],
                         mydata.data.Data.size(), 0, mydata.data.Offset,
                         2 * sizeof(float));
      }
      ImPlot::EndPlot();
    }
  }
  ImGui::End();
}