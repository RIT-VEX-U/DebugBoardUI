#include "Visualization/GenericGrapher.hpp"
#include "../Types.hpp"
#include "GraphUtils.hpp"
#include "Widget.hpp"
#include "imgui.h"
#include "implot/implot.h"
#include <chrono>
#include <cstddef>
#include <format>
#include <print>
#include <string>
#include <vector>

GenericGrapher::GenericGrapher(WidgetId id)
    : WidgetImpl(id), creation_time_(Timestamp::clock::now()) {}

void GenericGrapher::ReceiveData(TimedData new_data) {
  std::expected<double, DataRetrieveFailure> time =
      getDoubleAt(time_loc_, new_data);
  std::printf("time path: %s\n", time_loc_.toString().c_str());
  if (!time.has_value()) {
    for (auto [k, v] : new_data) {
      // std::println("{}:{}", k, v);
    }
    std::println("Ignoring data packet, no time: {}", (int)time.error());
    return;
  }
  double t = time.value();
  time_data_.AddPoint(t);
  for (AxisData &axd : data_) {
    if (axd.loc.isEmpty()) {
      continue;
    }
    if (!new_data.contains(axd.loc)) {
      throw new std::logic_error(std::format(
          "I requested {} to be included in this update but didnt get it",
          axd.loc.toString()));
    } else {
      const DataPrimitive &value = new_data[axd.loc].value;
      double vald = std::get<double>(value);
      axd.data.AddPoint((float)vald);
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
      ImGui::TextUnformatted(
          std::format("{} samples", data_[i].data.Data.size()).c_str());
      ImGui::SameLine();
      if (ImGui::InputText("Input", input_buffer, IM_ARRAYSIZE(input_buffer))) {
        SendingData data_to_send{
            .data = input_buffer,
            .loc = data_[i].loc,
        };
        input_data.push_back(data_to_send);
      }
    }
    if (ImGui::Button("Add Series")) {
      data_.push_back(AxisData{
          .secondary_y = false,
          .loc = DataLocator{},
          .data = ScrollingBuffer<float>{},
      });
    }
    // if (ImGui::InputFloat("Float", curr_data)) {
    //   float new_data = *curr_data;
    //   input_data.push_back(new_data);
    // }
    if (reregister) {
      ClearAndReregister();
    }
  }
  ImGui::End();

  auto min = [](float a, float b) { return (a < b) ? a : b; };
  auto max = [](float a, float b) { return (a > b) ? a : b; };
  if (ImGui::Begin(plotname.c_str())) {
    ImPlotAxisFlags const flags = 0;
    // ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoTickMarks;

    if (ImPlot::BeginPlot("Scrolling", ImVec2(-1, 250))) {
      ImPlot::SetupAxes("Scrolling Title a", "Scrolling Title b", flags, flags);
      if (!data_.empty() && !time_data_.Data.empty()) {
        // float then = time_.Oldest();
        history = 60;
        float now = time_data_.Recent();
        float old = time_data_.Oldest();
        if (now - history > 0) {
          old = now - history;
        }
        std::println("old: {} now: {}", old, now);
        ImPlot::SetupAxisLimits(ImAxis_X1, old, now, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1);
        ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5F);

        for (auto &i : data_) {
          const AxisData &mydata = i;
          if (mydata.data.Data.empty() || i.loc.isEmpty()) {
            // skip invalid guys
            continue;
          }
          // whatthefuck im gonna data all over this data
          ImPlot::PlotLine("Data", time_data_.Data.data(),
                           mydata.data.Data.data(), mydata.data.Data.size(), 0,
                           mydata.data.Offset, sizeof(float));
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

void GenericGrapher::ClearAndReregister() {
  DataLocationSet wanted_data{};
  wanted_data.reserve(data_.size());
  for (auto &series : data_) {
    wanted_data.emplace(series.loc);
    series.data.Erase();
  }
  wanted_data.emplace(time_loc_);
  time_data_.Erase();

  RegisterDataCallback(wanted_data);
}
