#pragma once
#include "../Types.hpp"
#include "GraphUtils.hpp"
#include "Widget.hpp"
#include "imgui.h"
class PidVizWidget : public WidgetImpl {
public:
  PidVizWidget(WidgetId id);
  ~PidVizWidget();

  void ReceiveData(DataElement data) override;
  // Use ImGui to draw a this widget
  void Draw(bool *should_close) override;

private:
  DataLocator t_loc;
  DataLocator sp_loc;
  DataLocator pv_loc;

  bool plot_error_;
  double t;
  ScrollingBuffer<ImVec2> sdata;
};