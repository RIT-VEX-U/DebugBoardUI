#pragma once
#include "../Types.hpp"
#include "GraphUtils.hpp"
#include "Widget.hpp"
#include "imgui.h"
class PidVizWidget : public WidgetImpl {
public:
  explicit PidVizWidget(WidgetId id);

  void ReceiveData(TimedData data) override;
  // Use ImGui to draw a this widget
  void Draw(bool *should_close) override;

private:
  DataLocator t_loc_;
  DataLocator sp_loc_;
  DataLocator pv_loc_;

  bool plot_error_;
  double t_;
  ScrollingBuffer<ImVec2> sdata_;
};
