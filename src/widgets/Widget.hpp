#pragma once
#include "../Types.hpp"
#include <functional>
#include <memory>
#include <string>

using WidgetId = std::size_t;

class WidgetImpl;
using Widget = std::shared_ptr<WidgetImpl>;
using DefaultWidgetCreator = std::function<Widget(WidgetId)>;

class WidgetImpl {
public:
  WidgetImpl(WidgetId id);
  virtual ~WidgetImpl() {}
  void RegisterDataCallback(std::vector<DataLocator> wanted_data);

  virtual void ReceiveData(DataElement data) = 0;
  // Use ImGui to draw a this widget
  virtual void Draw(bool *should_close) = 0;

  WidgetId Id();

  std::vector<DataLocator> WantedData() const;

private:
  WidgetId id_;
  std::vector<DataLocator> wanted_data_;
};
