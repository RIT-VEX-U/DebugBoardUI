#include "ControlSystems/PidVizWidget.hpp"
#include "Visualization/FieldMapWidget.hpp"
#include "Visualization/GenericGrapher.hpp"
#include "Widget.hpp"
#include "WidgetRegistry.hpp"
#include <memory>

// Add your widgets here
void AddWidgetsToCollection(WidgetRegistry &reg) {
  reg.RegisterWidget("PID Visualizer", [](WidgetId id) {
    return std::make_shared<PidVizWidget>(id);
  });
  reg.RegisterWidget("Field Map", [](WidgetId id) {
    return std::make_shared<FieldMapWidget>(id, "high_stake.png");
  });
  reg.RegisterWidget("Generic Grapher", [](WidgetId id) {
    return std::make_shared<GenericGrapher>(id);
  });
}