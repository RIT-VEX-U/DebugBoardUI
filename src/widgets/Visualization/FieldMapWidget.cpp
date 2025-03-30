#include "Visualization/FieldMapWidget.hpp"
#include "/home/unknown/Clubs/VEX/Code/DebugBoardUI/src/Types.hpp"
#include "Widget.hpp"
#include "imgui.h"
#include <string>
FieldMapWidget::FieldMapWidget(WidgetId id, const std::string & /*field_image_path*/)
    : WidgetImpl(id)
    , flipDiagonally(false)
{}
void FieldMapWidget::ReceiveData(DataElement data) {}

void FieldMapWidget::Draw(bool *should_close) {
  if (ImGui::Begin("Field Map", should_close)) {
    ImGui::Checkbox("Flip Diagonally", &flipDiagonally);
  }
  ImGui::End();
}