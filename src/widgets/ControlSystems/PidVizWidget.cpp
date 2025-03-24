#include "ControlSystems/PidVizWidget.hpp"
#include <format>

PidVizWidget::PidVizWidget() {}
PidVizWidget::~PidVizWidget() {}
// void PidVizWidget::RegisterDataCallback() {}

void PidVizWidget::ReceiveData() { std::puts(std::format("Hellooo").c_str()); }
void PidVizWidget::Draw() {}