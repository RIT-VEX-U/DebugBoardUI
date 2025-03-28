#pragma once
#include "Widget.hpp"
class FieldMapWidget : public WidgetImpl {
public:
  FieldMapWidget(WidgetId id, std::string field_image_path);

  void ReceiveData(DataElement data) override;
  void Draw() override;

private:
  bool flipDiagonally;
};