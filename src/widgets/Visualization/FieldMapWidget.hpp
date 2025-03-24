#pragma once
#include "Widget.hpp"
class FieldMapWidget : public WidgetImpl {
public:
  FieldMapWidget(std::string field_image_path);

  void ReceiveData() override;
  void Draw() override;

private:
  bool flipDiagonally;
};