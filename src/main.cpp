#include "imgui.h"

#include "platform/glue.hpp"
#include <math.h>

#include "Workspace.hpp"
#include "widgets/WidgetRegistry.hpp"
#include <cstdio>
#include <format>
#include <string>

int main() {
  Workspace::Init();

  Platform::Data pdata = Platform::init("Debug Board UI");

  // Main loop
  while (!Platform::shouldclose(pdata)) {
    Platform::prerender(pdata);

    Workspace::Draw();

    Platform::postrender(pdata);
  }

  Platform::cleanup(pdata);
}