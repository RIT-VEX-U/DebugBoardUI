#pragma once
#include "Widget.hpp"
class FieldMapWidget : public WidgetImpl {
public:
    FieldMapWidget(WidgetId id, const std::string &field_image_path);

    void ReceiveData(DataElement data) override;
    void Draw(bool *) override;

private:
  bool flipDiagonally;
};