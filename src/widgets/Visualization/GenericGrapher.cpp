#include "Visualization/GenericGrapher.hpp"
#include "../Types.hpp"
#include "GraphUtils.hpp"
#include "Widget.hpp"
#include "imgui.h"
#include "implot/implot.h"
#include <chrono>
#include <cstddef>
#include <format>
#include <string>
#include <vector>
#include <print>

GenericGrapher::GenericGrapher(WidgetId id)
    : WidgetImpl(id), creation_time_(Timestamp::clock::now())
{
}

void GenericGrapher::ReceiveData(TimedData new_data) {
    Timestamp ts{};
    for (AxisData &axd : data_){
        if (axd.loc.isEmpty() ){
            continue;
        }
        if (!new_data.contains(axd.loc)){
            throw new std::logic_error(std::format("I requested {} to be included in this update but didnt get it", axd.loc.toString()));
        } else {
            ts = new_data[axd.loc].time;
            std::chrono::duration<float> t_seconds(ts - creation_time_);

            const DataPrimitive &value = new_data[axd.loc].value;
            double vald = std::get<double>(value);
            axd.data.AddPoint(Pt{t_seconds.count(), (float)vald});
        }
    }

}
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
            ImGui::SameLine();
            ImGui::TextUnformatted(std::format("{} samples", data_[i].data.Data.size()).c_str());
        }
        if (ImGui::Button("Add Series")) {
            data_.push_back(AxisData{
                .secondary_y = false,
                .loc = DataLocator{},
                .data = ScrollingBuffer<Pt>{},
            });
        }
        if (reregister) {
            DataLocationSet wanted_data{};
            wanted_data.reserve(data_.size());
            for (const auto &series : data_) {
                wanted_data.emplace(series.loc);
            }
            RegisterDataCallback(wanted_data);
        }
  }
  ImGui::End();

  auto min = [](float a, float b){return (a < b)? a : b;};
  if (ImGui::Begin(plotname.c_str())) {
      ImPlotAxisFlags const flags = 0;
      // ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoTickMarks;

      if (ImPlot::BeginPlot("Scrolling", ImVec2(-1, 250))) {
          ImPlot::SetupAxes("Scrolling Title a", "Scrolling Title b", flags, flags);
          if (data_.size() !=0){
          // float then = time_.Oldest();
          float now = data_[0].data.Recent().x;
          float old = data_[0].data.Oldest().x;
          old = min(old, now- 5.0);
          ImPlot::SetupAxisLimits(ImAxis_X1, old, now, ImGuiCond_Always);
          ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1);
          ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5F);
          for (auto &i : data_) {
              const AxisData &mydata = i;
              if (mydata.data.Data.empty() || i.loc.isEmpty()) {
                  // skip invalid guys
                  continue;
              }


              ImPlot::PlotLine("Data", &mydata.data.Data[0].x, &mydata.data.Data[0].y,
                               mydata.data.Data.size(), 0, mydata.data.Offset, 2 * sizeof(float));
              // ImPlot::PlotLine("Data",
              //                  time_.Data.data(),
              //                  mydata.data.Data.data(),
              //                  mydata.data.Data.size(),
              //                  0,
              //                  mydata.data.Offset,
              //                  2 * sizeof(float));
          }
          }
          ImPlot::EndPlot();
      }
  }
  ImGui::End();
}
