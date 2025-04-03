#include "platform/glue.hpp"

#include "Workspace.hpp"
#include "datasources/DebugBoard.hpp"
#include <cstdio>
#include <memory>
#include <print>
int main() {
  Platform::Data pdata = Platform::init("Debug Board UI");
  std::println("Init");
  // auto board =
  // std::make_shared<DebugBoardWebsocket>("ws://localhost:8080/ws");
  auto board = std::make_shared<DebugBoardWebsocket>("ws://10.115.37.28/ws");
  Workspace::AddSource(board);
  Workspace::Init();


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
