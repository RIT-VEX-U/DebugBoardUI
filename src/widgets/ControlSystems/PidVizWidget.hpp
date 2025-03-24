#pragma once

#include "Widget.hpp"
class PidVizWidget : public WidgetImpl {
public:
  PidVizWidget();
  ~PidVizWidget();
  void RegisterDataCallback();

  void ReceiveData() override;
  // Use ImGui to draw a this widget
  void Draw() override;
};