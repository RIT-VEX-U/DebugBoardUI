#include "datasources/DebugBoard.hpp"
#include "imgui.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

DebugBoard::DebugBoard() {}

DebugBoard::~DebugBoard() {}

std::vector<DataUpdate> DebugBoard::PollData() {
  std::vector<DataUpdate> updates{};
  // return out list of unread stuff, reset internal list
  std::swap(updates, unread_updates);
  return updates;
}

DebugBoard::DataElementSet DebugBoard::ProvidedData() const {
  return current_channels;
}
void DebugBoard::feedPacket(const std::string &json_obj) {
  json data = json::parse(json_obj);
  feedPacket(data);
}

bool isAdvertise(const json &obj) {
  if (!obj.contains("type")) {
    return false;
  }
  return obj["type"] == "advertise";
}

bool isData(const json &obj) {
  if (!obj.contains("type")) {
    return false;
  }
  return obj["type"] == "data";
}
void DebugBoard::feedPacket(const json &json_obj) {
  if (isAdvertise(json_obj)) {
    printf("advertise\n");
  } else {
    printf("Not advertise\n");
  }
}

// DebugBoardWebsocket::~DebugBoardWebsocket() {}
DebugBoardWebsocket::DebugBoardWebsocket(const std::string &ws_url,
                                         TimeDuration retry_period)
    : ws_url_(ws_url), ws_(easywsclient::WebSocket::from_url(ws_url)) {

  last_connect_time_ = std::chrono::steady_clock::now();
}

std::vector<DataUpdate> DebugBoardWebsocket::PollData() {
  if (ws_ == nullptr) {
    // TODO: maybe handle reconnect
    return DebugBoard::PollData();
  }

  ws_->poll();
  ws_->dispatch([&](std::string msg) { DebugBoard::feedPacket(msg); });

  return DebugBoard::PollData();
}

std::string DebugBoardWebsocket::Name() const {
  return "Debug Board at" + ws_url_;
}

void DebugBoardWebsocket::Draw() {}