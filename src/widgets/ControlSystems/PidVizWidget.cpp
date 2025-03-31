#include "ControlSystems/PidVizWidget.hpp"
#include "Widget.hpp"
#include "imgui.h"
#include "implot/implot.h"
#include <../Types.hpp>
#include <cmath>
#include <print>

PidVizWidget::PidVizWidget(WidgetId id)
    : WidgetImpl(id), plot_error_(false), t_(0) {}

void PidVizWidget::ReceiveData(TimedData data) {
}
void PidVizWidget::Draw(bool *should_close) {
    if (ImGui::Begin("Pid Visualizer", should_close)) {
  }

  ImGui::End();
}
