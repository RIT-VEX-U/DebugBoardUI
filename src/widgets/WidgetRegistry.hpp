#pragma once
#include "Widget.hpp"
class WidgetRegistry {
public:
  using MapType = std::unordered_map<std::string, DefaultWidgetCreator>;
  WidgetRegistry();
  void RegisterWidget(std::string name, DefaultWidgetCreator creator);
  MapType::iterator begin();
  MapType::iterator end();

  MapType::const_iterator cbegin() const;
  MapType::const_iterator cend() const;

private:
  MapType registered_widgets;
};

void AddWidgetsToCollection(WidgetRegistry &reg);