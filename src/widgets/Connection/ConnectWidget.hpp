#pragma once
#include <cstdint>
#include <memory>
#include "../Types.hpp"
#include "Widget.hpp"
#include "Workspace.hpp"
#include "datasources/DebugBoard.hpp"
#include "imgui.h"
class ConnectWidget : public WidgetImpl {
public:
  explicit ConnectWidget(WidgetId id);

  void ReceiveData(TimedData data) override;
  // Use ImGui to draw a this widget
  void Draw(bool *should_close) override;

private:
    bool board_created = false;
    std::shared_ptr<DebugBoardWebsocket> board;
    char url_buf[128] = "ws://debugboard.student.rit.edu/ws";
};