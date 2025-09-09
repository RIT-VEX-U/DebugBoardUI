#pragma once
#include "Widget.hpp"
#include "stb/stb_image.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "platform/glue.hpp"
#include <array>
class FieldMapWidget : public WidgetImpl {
public:
    struct OdomData {
    DataLocator loc = {};
  };
  FieldMapWidget(WidgetId id, const std::string &field_image_path);

  void ReceiveData(TimedData data) override;
  double PixelsPerInch();
  void Draw(bool *) override;
  ImVec2 RotatePoint(const ImVec2& p, const ImVec2& center, float angleRad);
  void DrawRotatedRect(ImDrawList* draw_list, ImVec2 center, ImVec2 size, float angleRad, ImU32 color);
  void ClearAndReregister();

private:
  bool flipDiagonally;
  GLuint field_texture = 0;
  GLuint bot_texture = 0;
  int field_squaredres_resized = 1000;
  int field_squaredres = field_squaredres_resized;
  int bot_squaredres = field_squaredres_resized * 0.104;
  ImVec2 field_size = ImVec2(field_squaredres_resized, field_squaredres_resized);
  ImVec2 bot_size = ImVec2(field_squaredres_resized * 0.104, field_squaredres_resized * 0.104);
  ImVec4 bot_color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
  double botX = 24;
  double botY = 72;
  double botRot = 0;
  char xBuffer[128] = "";
  char yBuffer[128] = "";
  char rotBuffer[128] = "";
  OdomData BotXData;
  OdomData BotYData;
  OdomData BotRotData;
};