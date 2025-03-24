#include "WidgetRegistry.hpp"

WidgetRegistry::WidgetRegistry() {}

void WidgetRegistry::RegisterWidget(std::string name,
                                    DefaultWidgetCreator creator) {
  registered_widgets[name] = creator;
}

WidgetRegistry::MapType::iterator WidgetRegistry::begin() {
  return registered_widgets.begin();
}
WidgetRegistry::MapType::iterator WidgetRegistry::end() {
  return registered_widgets.end();
}

WidgetRegistry::MapType::const_iterator WidgetRegistry::cbegin() const {
  return registered_widgets.cbegin();
}
WidgetRegistry::MapType::const_iterator WidgetRegistry::cend() const{
  return registered_widgets.cend();
}