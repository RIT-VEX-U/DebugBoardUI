#pragma once
#include "GraphUtils.hpp"
#include "Widget.hpp"
class PidVizWidget : public WidgetImpl {
public:
  PidVizWidget(WidgetId id);
  ~PidVizWidget();
  void RegisterDataCallback();

  void ReceiveData() override;
  // Use ImGui to draw a this widget
  void Draw() override;

private:
  bool plot_error_;
  double t;
  ScrollingBuffer sdata;
  RollingBuffer rdata;
};