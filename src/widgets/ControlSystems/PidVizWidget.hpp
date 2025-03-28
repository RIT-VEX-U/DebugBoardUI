#pragma once
#include "../Types.hpp"
#include "GraphUtils.hpp"
#include "Widget.hpp"
class PidVizWidget : public WidgetImpl {
public:
  PidVizWidget(WidgetId id);
  ~PidVizWidget();

  void ReceiveData(DataElement data) override;
  // Use ImGui to draw a this widget
  void Draw() override;

private:
  DataLocator t_loc;
  DataLocator sp_loc;
  DataLocator pv_loc;

  bool plot_error_;
  double t;
  ScrollingBuffer sdata;
  RollingBuffer rdata;
};