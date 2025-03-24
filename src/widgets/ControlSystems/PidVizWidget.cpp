#include "ControlSystems/PidVizWidget.hpp"
#include "imgui.h"

PidVizWidget::PidVizWidget(WidgetId id) : WidgetImpl(id) {}
PidVizWidget::~PidVizWidget() {}
// void PidVizWidget::RegisterDataCallback() {}

void PidVizWidget::ReceiveData() {}
void PidVizWidget::Draw() {
  if (ImGui::Begin("Pid Visualizer")) {
    ImGui::Checkbox("Plot error", &plot_error_);
  }

  ImGui::End();
}