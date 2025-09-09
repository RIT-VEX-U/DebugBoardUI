#include "Connection/ConnectWidget.hpp"
#include "Widget.hpp"
#include "imgui.h"
#include "implot/implot.h"
#include <../Types.hpp>
#include <cmath>
#include <print>

ConnectWidget::ConnectWidget(WidgetId id): WidgetImpl(id){
    board = std::make_shared<DebugBoardWebsocket>();
    Workspace::AddSource(board);
}

void ConnectWidget::ReceiveData(TimedData data) {
}

void ConnectWidget::Draw(bool *should_close) {
    if (ImGui::Begin("Connect to Board", should_close)) {
        if(ImGui::Button("Connect")){
            // board->ConnectWS(url_buf);
            board->ConnectAsync(url_buf);
        } 
    ImGui::SameLine();
    ImGui::Text("URL: ");
    ImGui::SameLine();
    if (ImGui::InputText("##", url_buf, sizeof(url_buf))) {
    }
  }
  ImGui::End();
}