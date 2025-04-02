#include "platform/glue.hpp"

#include "Workspace.hpp"
#include "datasources/DebugBoard.hpp"
#include <cstdio>
#include <memory>
#include <print>
int main() {
  std::println("Init");
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
