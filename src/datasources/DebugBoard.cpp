#include "datasources/DebugBoard.hpp"
#include "imgui.h"

DebugBoard::DebugBoard(std::string url)
    : url_(url), ws_(easywsclient::WebSocket::from_url(url)) {}

DebugBoard::~DebugBoard() {
  ws_->close();
  printf("Closing WS\n");
}

std::vector<DataUpdate> DebugBoard::PollData() {
  if (ws_ == nullptr) {
    return {};
  }
  ws_->poll(1);
  auto handler = [](const std::string &txt) {
    // printf("Got text %s\n", txt.c_str());
  };
  ws_->dispatch(handler);
  return {};
}

std::string DebugBoard::Name() const { return "Debug Board at " + url_; }

DebugBoard::ProvidedDataT DebugBoard::ProvidedData() const {
  DebugBoard::ProvidedDataT returnValue{};
  return returnValue;
}

void DebugBoard::Draw() {}