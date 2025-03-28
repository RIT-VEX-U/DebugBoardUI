#include "Visualization/FieldMapWidget.hpp"
#include "imgui.h"
FieldMapWidget::FieldMapWidget(WidgetId id, std::string field_image_path)
    : WidgetImpl(id), flipDiagonally(false) {}
void FieldMapWidget::ReceiveData(DataElement data) {}

void FieldMapWidget::Draw(bool *should_close) {
  if (ImGui::Begin("Field Map", should_close)) {
    ImGui::Checkbox("Flip Diagonally", &flipDiagonally);
  }
  ImGui::End();
}