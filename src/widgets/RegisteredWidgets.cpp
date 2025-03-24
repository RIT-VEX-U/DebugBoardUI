#include "ControlSystems/PidVizWidget.hpp"
#include "Visualization/FieldMapWidget.hpp"
#include "Widget.hpp"
#include "WidgetRegistry.hpp"

// Add your widgets here
void AddWidgetsToCollection(WidgetRegistry &reg) {
  reg.RegisterWidget("PID Visualizer", [](WidgetId id) {
    return std::make_shared<PidVizWidget>(id);
  });
  reg.RegisterWidget("Field Map", [](WidgetId id) {
    return std::make_shared<FieldMapWidget>(id, "high_stake.png");
  });
}