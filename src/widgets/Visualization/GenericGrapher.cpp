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
#include "Workspace.hpp"
/**
  * Creates a generic grapher with a widget id
  * @param id the widget id for the grapher to have
  */
GenericGrapher::GenericGrapher(WidgetId id)
    : WidgetImpl(id), creation_time_(Timestamp::clock::now()) {}

/**
  * Function for receiving data from the websocket
  * @param data the new data we have received
  */
void GenericGrapher::ReceiveData(TimedData new_data) {
  //gets the time data
  std::expected<double, DataRetrieveFailure> time = getDoubleAt(time_loc_, new_data);
  //double check that the time set actually has a time
  if (!time.has_value()) {
    for (auto [k, v] : new_data) {
      // std::println("{}:{}", k, v);
    }
    std::println("Ignoring data packet, no time: {}", (int)time.error());
    return;
  }
  //sets the time data
  double t = time.value();
  time_data_.AddPoint(t);
  for (AxisData &axd : data_) {
    //checks that the data locator is real;
    if (axd.loc.isEmpty()) {
      continue;
    }
    //checks that the data at the locator is real
    if (!new_data.contains(axd.loc)) {
      throw new std::logic_error(std::format("I requested {} to be included in this update but didnt get it", axd.loc.toString()));
    } 
    else {
      //gets the data value from the data locator
      const DataPrimitive &value = new_data[axd.loc].value;
      const DataPrimitiveType &type = new_data[axd.loc].type;
      double vald;
      if(type == DataPrimitiveType::Float){
        vald = std::get<double>(value);
        axd.data.AddPoint((float)vald);
      } else if(type == DataPrimitiveType::Int){
        std::println("int received, plot might not be accurate");
        vald = (double)std::get<int64_t>(value);
        axd.data.AddPoint((float)vald);
      } else if(type == DataPrimitiveType::Uint){
        std::println("uint received, plot might not be accurate");
        vald = (double)std::get<uint64_t>(value);\
        axd.data.AddPoint((float)vald);
      } else if(type == DataPrimitiveType::String){
        std::println("string received, not adding to plot");
      }
    }
  }
}
/**
 * Draws all the sections of the Generic Grapher
 * @param should_close whether or not we should close the grapher
 */
void GenericGrapher::Draw(bool *should_close) {
  std::string const dataname = std::format("Generic Drawer Inputs #{}", Id());
  std::string const plotname = std::format("Generic Drawer Plot #{}", Id());

  //begins running the generic grapher data grabber
  if (ImGui::Begin(dataname.c_str()), should_close != nullptr) {
    //creates the location selector for the time
    DataLocationSelector("Time", time_loc_);

    ImGui::Separator();
    bool reregister = false;
    //sets up input buffers for inputting text later
    if(input_buffers.size() < data_.size()){
      size_t old_size = input_buffers.size();
      input_buffers.resize(data_.size());

      for (size_t i = old_size; i < data_.size(); ++i) {
        input_buffers[i].fill('\0');
      }
    }

    //loops through all the axis data
    for (size_t i = 0; i < data_.size(); i++) {
      //Data selector for the data we are looking for
      std::string const name = std::format("Series {}", i);
      //checks if the sources to select from has changed
      reregister |= DataLocationSelector(name.c_str(), data_[i].loc);
      ImGui::SameLine();

      //Text displaying how many samples we have retreived from the data locator we are looking at
      ImGui::TextUnformatted(std::format("{} samples", data_[i].data.Data.size()).c_str());
      ImGui::SameLine();
      
      
      //automatically resizes the text box
      float text_width = ImGui::CalcTextSize(input_buffers[i].data()).x + 20;
      if(text_width < 60){
        text_width = 60;
      }
      ImGui::SetNextItemWidth(text_width);

      //sets up the id for the text input
      ImGui::PushID(i);
      std::string label = "##" + std::to_string(i);
      //Input text for data so we can send data back to the webserver if we want
      if (ImGui::InputText(label.c_str(), input_buffers[i].data(), input_buffers[i].size(), ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue)) {
        //gets the data input into the text box
        SendingData data_to_send{
            .waiting_to_send = true,
            .data = input_buffers[i].data(),
            .data_type = Float,
            .loc = data_[i].loc,
        };
        
        // printf("Data Inputted: %s, Data Input Locator: %s\n", input_buffer, data_[i].loc.path.parts[data_[i].loc.path.parts.size() - 1]);
        //adds the data to the queue to send to the webserver
        // printf("Data to send Entered!\n");
        SendToWebSocketCallback(data_to_send);
      }
      ImGui::PopID();
    }
    //Button to add a new series of data to the grapher
    if (ImGui::Button("Add Series")) {
      data_.push_back(AxisData{
          .secondary_y = false,
          .loc = DataLocator{},
          .data = ScrollingBuffer<float>{},
      });
    }
    //checks if we need to reregister any data
    if (reregister) {
      ClearAndReregister();
    }
  }
  ImGui::End();

  auto min = [](float a, float b) { return (a < b) ? a : b; };
  auto max = [](float a, float b) { return (a > b) ? a : b; };
  //Begins running the Generic Grapher Graph Scroller
  if (ImGui::Begin(plotname.c_str())) {

    ImPlotAxisFlags const flags = 0;

    //Begins the scrolling graph plot
    if (ImPlot::BeginPlot("Scrolling", ImVec2(-1, 250))) {

      //creates the axis for the graph
      // ImPlot::SetupAxes("Scrolling Title a", "Scrolling Title b", flags, flags);

      static float range_min = 0.0f;
      static float range_max = 10.0f;

      // Input boxes for range
      ImGui::InputFloat("Range Min", &range_min, 0.0f, 0.0f, "%.3f");
      ImGui::InputFloat("Range Max", &range_max, 0.0f, 0.0f, "%.3f");

      // Clamp so min < max
      if (range_max <= range_min) {
          range_max = range_min + 0.1f;
      }

      //checks that the data and time data are both real
      if (!data_.empty() && !time_data_.Data.empty()) {

        //sets the scrolling plot's most recent and oldest data
        float now = time_data_.Recent();
        float old = time_data_.Oldest();
        //cuts off the oldest data at the end of the history
        if (now - history > 0) {
          old = now - history;
        }
        // std::println("old: {} now: {}", old, now);

        //Sets the axis limits using the now and old data
        ImPlot::SetupAxisLimits(ImAxis_X1, old, now, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, range_min, range_max, ImGuiCond_Always);
        ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5F);

        //goes through all the data inside our found data
        for (auto &i : data_) {
          const AxisData &mydata = i;
          //double checks that the data is real
          if (mydata.data.Data.empty() || i.loc.isEmpty()) {
            // skip invalid guys
            continue;
          }
          //plots the line of data using the data we found and the time we have
          //whatthefuck im gonna data all over this data
          ImPlot::PlotLine("Data", time_data_.Data.data(), mydata.data.Data.data(), mydata.data.Data.size(), 0, mydata.data.Offset, sizeof(float));
        }
      }
      ImPlot::EndPlot();
    }
  }
  ImGui::End();
}

/**
 * Function for Clearing and Reregistering the data in the grapher 
 */
void GenericGrapher::ClearAndReregister() {
  DataLocationSet wanted_data{};
  //reserves the space in the location set for the data we want
  wanted_data.reserve(data_.size());

  //go through each series in the data we have
  for (auto &series : data_) {
    //put the series in the data we have into the location set of data we want
    wanted_data.emplace(series.loc);
    //erases the series data since we no longer need it
    series.data.Erase();
  }
  //puts the time location into the data we want
  wanted_data.emplace(time_loc_);
  //erases the data for the time that we no longer need
  time_data_.Erase();
  //Sends the wanted data to the "Router" to let it know what we are looking for
  RegisterDataCallback(wanted_data);
}
