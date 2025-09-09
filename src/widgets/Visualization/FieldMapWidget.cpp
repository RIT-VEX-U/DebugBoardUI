#include "Visualization/FieldMapWidget.hpp"
#include "../Types.hpp"
#include "Widget.hpp"
#include <string>
#include <cmath>
#include <print>
FieldMapWidget::FieldMapWidget(WidgetId id, const std::string & /*field_image_path*/)
    : WidgetImpl(id)
    , flipDiagonally(false){
  bool field_image_loaded = Platform::LoadTextureFromFile("../src/widgets/Visualization/Field_Map.png", &field_texture, &field_squaredres, &field_squaredres);
  IM_ASSERT(field_image_loaded);
  bool bot_image_loaded = Platform::LoadTextureFromFile("../src/widgets/Visualization/Bot.png", &bot_texture, &bot_squaredres, &bot_squaredres);
  IM_ASSERT(bot_image_loaded);
}

void FieldMapWidget::ReceiveData(TimedData data) {
  if (!BotXData.loc.isEmpty()) {
    std::expected<double, DataRetrieveFailure> botXNew = getDoubleAt(BotXData.loc, data);
      botX = botXNew.value();
      std::println("BOT X NEW VALUE: {}", botXNew.value());
  }
  if(!BotYData.loc.isEmpty()){
    std::expected<double, DataRetrieveFailure> botYNew = getDoubleAt(BotYData.loc, data);
      botX = botYNew.value();
      std::println("BOT Y NEW VALUE: {}", botYNew.value());
  }
  if(!BotRotData.loc.isEmpty()){
    std::expected<double, DataRetrieveFailure> botRotNew = getDoubleAt(BotRotData.loc, data);
      botRot = botRotNew.value();
      std::println("BOT X NEW VALUE: {}", botRotNew.value());
  }
}

/**
 * Function for Clearing and Reregistering the data in the grapher 
 */
void FieldMapWidget::ClearAndReregister() {
  DataLocationSet wanted_data{};
  std::vector<OdomData> data_;
  data_.push_back(BotXData);
  data_.push_back(BotYData);
  data_.push_back(BotRotData);
  //reserves the space in the location set for the data we want
  wanted_data.reserve(data_.size());

  //go through each series in the data we have
  for (auto &series : data_) {
    //put the series in the data we have into the location set of data we want
    wanted_data.emplace(series.loc);
    //erases the series data since we no longer need it
  }
  //Sends the wanted data to the "Router" to let it know what we are looking for
  RegisterDataCallback(wanted_data);
}

void FieldMapWidget::Draw(bool *should_close) {

  if (ImGui::Begin("Field Map", should_close)) {
    ImGui::Image((ImTextureID)(intptr_t)field_texture, field_size);
    ImVec2 field_upper_left = ImGui::GetItemRectMin();
    ImVec2 field_lower_right = ImGui::GetItemRectMax();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    ImGui::SameLine();
    ImGui::BeginGroup();
    DataLocationSelector("Bot X", BotXData.loc);
    ImGui::SameLine();
    float xBufferWidth = ImGui::CalcTextSize(xBuffer).x + 20;
    if(xBufferWidth < 60){
      xBufferWidth = 60;
    }
    ImGui::SetNextItemWidth(xBufferWidth);
    ImGui::PushID(0);
    if (ImGui::InputText("##", xBuffer, IM_ARRAYSIZE(xBuffer), ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue)) {
      char *endPtr;
      botRot = strtod(rotBuffer, &endPtr);
      if (rotBuffer == endPtr) {
        std::println("Input was not a double, please enter a double");
      }
      else{
        //gets the data input into the text box
        SendingData data_to_send{
            .waiting_to_send = true,
            .data = xBuffer,
            .data_type = Float,
            .loc = BotXData.loc,
        };
        
        // printf("Data Inputted: %s, Data Input Locator: %s\n", input_buffer, data_[i].loc.path.parts[data_[i].loc.path.parts.size() - 1]);
        //adds the data to the queue to send to the webserver
        // printf("Data to send Entered!\n");
        SendToWebSocketCallback(data_to_send);
      }
    }
    ImGui::PopID();


    DataLocationSelector("Bot Y", BotYData.loc);
    ImGui::SameLine();
    float yBufferWidth = ImGui::CalcTextSize(yBuffer).x + 20;
    if(yBufferWidth < 60){
      yBufferWidth = 60;
    }
    ImGui::SetNextItemWidth(yBufferWidth);
    ImGui::PushID(1);
    if (ImGui::InputText("##", yBuffer, IM_ARRAYSIZE(yBuffer), ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue)) {
      char *endPtr;
      botRot = strtod(rotBuffer, &endPtr);
      if (rotBuffer == endPtr) {
        std::println("Input was not a double, please enter a double");
      }
      else{
        //gets the data input into the text box
        SendingData data_to_send{
            .waiting_to_send = true,
            .data = yBuffer,
            .data_type = Float,
            .loc = BotYData.loc,
        };
      
        // printf("Data Inputted: %s, Data Input Locator: %s\n", input_buffer, data_[i].loc.path.parts[data_[i].loc.path.parts.size() - 1]);
        //adds the data to the queue to send to the webserver
        // printf("Data to send Entered!\n");
        SendToWebSocketCallback(data_to_send);
      }
      
    }
    ImGui::PopID();
    DataLocationSelector("Bot Rotation", BotRotData.loc);
    ImGui::SameLine();
    float rotBufferWidth = ImGui::CalcTextSize(rotBuffer).x + 20;
    if(rotBufferWidth < 60){
      rotBufferWidth = 60;
    }
    ImGui::SetNextItemWidth(rotBufferWidth);
    ImGui::PushID(2);
    if (ImGui::InputText("##", rotBuffer, IM_ARRAYSIZE(rotBuffer), ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue)) {
      char *endPtr;
      botRot = strtod(rotBuffer, &endPtr);
      if (rotBuffer == endPtr) {
        std::println("Input was not a double, please enter a double");
      }
      else{
        //gets the data input into the text box
        SendingData data_to_send{
            .waiting_to_send = true,
            .data = yBuffer,
            .data_type = Float,
            .loc = BotRotData.loc,
        };
      
        // printf("Data Inputted: %s, Data Input Locator: %s\n", input_buffer, data_[i].loc.path.parts[data_[i].loc.path.parts.size() - 1]);
        //adds the data to the queue to send to the webserver
        // printf("Data to send Entered!\n");
        SendToWebSocketCallback(data_to_send);
      }
    }
    ImGui::PopID();
    ImGui::Text("Bot Color");
    ImGui::SameLine();
    ImGui::ColorEdit3("##", (float*)&bot_color, ImGuiColorEditFlags_NoInputs);
    ImGui::EndGroup();
    ImVec2 content_tl = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin();

    ImVec2 mouse_pos = ImGui::GetMousePos();
    ImVec2 window_pos = ImGui::GetWindowPos();

    ImVec2 local_pos = ImVec2(mouse_pos.x - window_pos.x, mouse_pos.y - window_pos.y);

    ImVec2 bot_pixel_center = ImVec2(field_upper_left.x + (botX * PixelsPerInch()), field_lower_right.y - std::round(botY * PixelsPerInch()));


    ImGui::Text("Mouse in window: (%.1f, %.1f)", local_pos.x, local_pos.y);
    ImGui::Text("Field Lower Right: (%.1f, %.1f)", field_lower_right.x - ImGui::GetWindowPos().x, field_lower_right.y - ImGui::GetWindowPos().y);
    ImGui::Text("Field Upper Left: (%.1f, %.1f)", field_upper_left.x - ImGui::GetWindowPos().x, field_upper_left.y - ImGui::GetWindowPos().y);
    ImGui::Text("Bot Pixel Center: (%.1f, %.1f)", bot_pixel_center.x, bot_pixel_center.y);
    ImGui::Text("Bot Real Loc: (%.1f, %.1f)", botX, botY);
    ImGui::Text("Field Squared Res and Pixels Per Inch: (%d, %.2f)", field_squaredres, PixelsPerInch());
    DrawRotatedRect(draw_list, bot_pixel_center, bot_size, -(botRot * (M_PI / 180)), ImGui::ColorConvertFloat4ToU32(bot_color));

    ImGui::Checkbox("Flip Diagonally", &flipDiagonally);
  }
  ImGui::End();
}

// Helper to rotate a point around a pivot
ImVec2 FieldMapWidget::RotatePoint(const ImVec2& p, const ImVec2& center, float angleRad) {
    float sin = sinf(angleRad);
    float cos = cosf(angleRad);

    // Translate point back to origin
    ImVec2 pt = p - center;

    // Rotate
    float xnew = pt.x * cos - pt.y * sin;
    float ynew = pt.x * sin + pt.y * cos;

    // Translate back
    return ImVec2(xnew + center.x, ynew + center.y);
}

double FieldMapWidget::PixelsPerInch(){
  double pixels_per_inch = (double)field_squaredres_resized / 144;
  return pixels_per_inch;
}

void FieldMapWidget::DrawRotatedRect(ImDrawList* draw_list, ImVec2 center, ImVec2 size, float angleRad, ImU32 color) {
    ImVec2 half = size * 0.5f;

    // Original axis-aligned corners
    ImVec2 corners[4] = {
        ImVec2(center.x - half.x, center.y - half.y),
        ImVec2(center.x + half.x, center.y - half.y),
        ImVec2(center.x + half.x, center.y + half.y),
        ImVec2(center.x - half.x, center.y + half.y),
    };

    // Rotate each corner
    for (int i = 0; i < 4; i++) {
        corners[i] = RotatePoint(corners[i], center, angleRad);
    }

    // Draw filled rect
    draw_list->AddImageQuad(bot_texture, corners[0], corners[1], corners[2], corners[3], ImVec2(0,0), ImVec2(1,0), ImVec2(1,1), ImVec2(0,1), color);}
