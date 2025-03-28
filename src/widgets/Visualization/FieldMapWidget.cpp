#include "Visualization/FieldMapWidget.hpp"
#include "imgui.h"
FieldMapWidget::FieldMapWidget(WidgetId id, std::string field_image_path)
    : WidgetImpl(id), flipDiagonally(false) {}
void FieldMapWidget::ReceiveData(DataElement data) {}

void FieldMapWidget::Draw() {
  if (ImGui::Begin("Field Map")) {
    ImGui::Checkbox("Flip Diagonally", &flipDiagonally);
  }
  ImGui::End();
}