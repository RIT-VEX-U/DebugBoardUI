#include "imgui.h"

#include "platform/glue.hpp"
#include <math.h>

#include "Workspace.hpp"
#include "datasources/DebugBoard.hpp"
#include "nlohmann/json.hpp"
#include "widgets/WidgetRegistry.hpp"
#include <cstdio>
#include <format>
#include <string>

int main() {
  DebugBoard board{"ws://localhost:8080/ws"};
  Workspace::Init();

  Platform::Data pdata = Platform::init("Debug Board UI");

  // Main loop
  while (!Platform::shouldclose(pdata)) {
    board.poll();

    Platform::prerender(pdata);

    Workspace::Draw();

    Platform::postrender(pdata);
  }

  Platform::cleanup(pdata);
}