#include "imgui.h"

#include "platform/glue.hpp"
#include <math.h>

#include "Workspace.hpp"
#include "widgets/ControlSystems/PidVizWidget.hpp"
#include "widgets/GraphUtils.hpp"
#include "widgets/Widget.hpp"
#include "widgets/WidgetRegistry.hpp"
#include <cstdio>
#include <format>
#include <string>

int main() {
  Workspace::Init();

  Platform::Data pdata = Platform::init("Debug Board UI");

  // Our state
  bool show_demo_window = false;
  bool show_plot_window = true;

  // Main loop
  while (!Platform::shouldclose(pdata)) {
    Platform::prerender(pdata);

    if (show_demo_window) {
      ImGui::ShowDemoWindow(&show_demo_window);
    }
    Workspace::Draw();

    Platform::postrender(pdata);
  }

  Platform::cleanup(pdata);
}