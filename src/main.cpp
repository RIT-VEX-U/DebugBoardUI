#include "platform/glue.hpp"

#include "Workspace.hpp"
#include "datasources/DebugBoard.hpp"
#include <cstdio>
#include <memory>

int main() {
  auto board = std::make_shared<DebugBoardWebsocket>("ws://localhost:8080/ws");
  // auto board =
  // std::make_shared<DebugBoardWebsocket>("ws://129.21.144.84/ws");
  Workspace::Init();
  Workspace::AddSource(board);

  Platform::Data pdata = Platform::init("Debug Board UI");

  // Main loop
  while (!Platform::shouldclose(pdata)) {
    // Polls sources and notifies widgets
    Workspace::RouteData();
    Platform::prerender(pdata);

    Workspace::Draw();

    Platform::postrender(pdata);
  }

  Platform::cleanup(pdata);
}