#pragma once
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
  //   void RegisterDataCallback();

  virtual void ReceiveData() = 0;
  // Use ImGui to draw a this widget
  virtual void Draw() = 0;

  WidgetId Id();

private:
  WidgetId id_;
};


