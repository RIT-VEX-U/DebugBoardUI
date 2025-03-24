#pragma once
#include <functional>
#include <memory>
#include <string>

class WidgetImpl;
using Widget = std::shared_ptr<WidgetImpl>;
using DefaultWidgetCreator = std::function<Widget()>;

class WidgetImpl {
public:
  virtual ~WidgetImpl() {}
  //   void RegisterDataCallback();

  virtual void ReceiveData() = 0;
  // Use ImGui to draw a this widget
  virtual void Draw() = 0;
};

void InitialRegisterWidgets();
void RegisterWidget(std::string name, DefaultWidgetCreator newDefault);

void OpenWidget(std::string widget_name);
void DrawNewWidgetUI();
