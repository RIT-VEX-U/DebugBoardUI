#include "Visualization/GenericGrapher.hpp"
#include "/home/unknown/Clubs/VEX/Code/DebugBoardUI/src/Types.hpp"
#include "GraphUtils.hpp"
#include "Widget.hpp"
#include "imgui.h"
#include "implot/implot.h"
#include <chrono>
#include <cstddef>
#include <format>
#include <string>
#include <vector>

GenericGrapher::GenericGrapher(WidgetId id)
    : WidgetImpl(id)
{}

void GenericGrapher::ReceiveData(DataElement el) {}
void GenericGrapher::Draw(bool *should_close) {
    std::string const dataname = std::format("Generic Drawer Inputs #{}", Id());
    std::string const plotname = std::format("Generic Drawer Plot #{}", Id());
    if (ImGui::Begin(dataname.c_str()), should_close != nullptr) {
        DataLocationSelector("Time", time_loc_);

        ImGui::Separator();
        bool reregister = false;
        for (size_t i = 0; i < data_.size(); i++) {
            std::string const name = std::format("Series {}", i);
            reregister |= DataLocationSelector(name.c_str(), data_[i].loc);
        }
        if (ImGui::Button("Add Series")) {
            data_.push_back(AxisData{
                .secondary_y = false,
                .loc = DataLocator{},
                .data = ScrollingBuffer<float>{},
            });
        }
        if (reregister) {
            std::vector<DataLocator> wanted_data{};
            wanted_data.reserve(data_.size());
            for (const auto &series : data_) {
                wanted_data.push_back(series.loc);
            }
            RegisterDataCallback(wanted_data);
        }
  }
  ImGui::End();

  if (ImGui::Begin(plotname.c_str())) {
      ImPlotAxisFlags const flags = 0;
      // ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoTickMarks;

      if (ImPlot::BeginPlot("Scrolling", ImVec2(-1, 250))) {
          ImPlot::SetupAxes("Scrolling Title a", "Scrolling Title b", flags, flags);
          auto t = std::chrono::duration<double>{most_recent_time_};

          ImPlot::SetupAxisLimits(ImAxis_X1, t.count() - 10.0, t.count(), ImGuiCond_Always);
          ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1);
          ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5F);
          for (auto &i : data_) {
              const AxisData &mydata = i;
              if (mydata.data.Data.empty() || i.loc.isEmpty()) {
                  // skip invalid guys
                  continue;
              }

              ImPlot::PlotLine("Data",
                               time_.Data.data(),
                               mydata.data.Data.data(),
                               mydata.data.Data.size(),
                               0,
                               mydata.data.Offset,
                               2 * sizeof(float));
          }
          ImPlot::EndPlot();
    }
  }
  ImGui::End();
}
