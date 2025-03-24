#include <functional>
#include <memory>
#include <string>

class WidgetImpl {
  virtual ~WidgetImpl() {}
  void RegisterDataCallback();

  virtual void ReceiveData();
  // Use ImGui to draw a this widget
  virtual void Draw();
};

using Widget = std::shared_ptr<WidgetImpl>;

void RegisterWidget(std::string name, std::function<Widget()> newDefault);
