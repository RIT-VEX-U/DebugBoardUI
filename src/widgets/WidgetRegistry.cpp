#include "WidgetRegistry.hpp"
#include "Widget.hpp"
#include <string>

#include <utility>

WidgetRegistry::WidgetRegistry() = default;

void WidgetRegistry::RegisterWidget(const std::string &name,
                                    DefaultWidgetCreator creator) {
    registered_widgets[name] = std::move(creator);
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