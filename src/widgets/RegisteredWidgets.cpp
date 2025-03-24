#include "ControlSystems/PidVizWidget.hpp"
#include "Visualization/FieldMapWidget.hpp"
#include "Widget.hpp"
// Add your widgets here
void InitialRegisterWidgets() {
  RegisterWidget("PID Visualizer",
                 []() { return std::make_shared<PidVizWidget>(); });
  RegisterWidget("Field Map", []() {
    return std::make_shared<FieldMapWidget>("high_stake.png");
  });
}