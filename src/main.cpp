#include "platform/glue.hpp"

#include "Workspace.hpp"
// #include "datasources/DebugBoard.hpp"
#include <cstdio>
#include <memory>
#include <print>
int main() {
  std::println("Initlializing Platform");
  Platform::Data pdata = Platform::init("Debug Board UI");
  std::println("Initializing Workspace");
  Workspace::Init();
  
  // Main loop
  while (!Platform::shouldclose(pdata)) {
    
    // Polls sources and notifies widgets
    // std::println("Routing Data");
    Workspace::RouteData();
    
    // std::println("Prerendering");
    Platform::prerender(pdata);

    // std::println("Drawing Workspace");
    Workspace::Draw();

    // std::println("PostRendering");
    Platform::postrender(pdata);
  }

  Platform::cleanup(pdata);
}