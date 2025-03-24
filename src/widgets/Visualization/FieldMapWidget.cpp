#include "Visualization/FieldMapWidget.hpp"
#include "imgui.h"
FieldMapWidget::FieldMapWidget(std::string field_image_path) {}
void FieldMapWidget::ReceiveData() {}
void FieldMapWidget::Draw() {
  ImGui::Begin("Field Map");
  ImGui::Checkbox("Flip Diagonally", &flipDiagonally);
  ImGui::End();
}