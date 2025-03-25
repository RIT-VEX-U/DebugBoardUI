#include "datasources/DebugBoard.hpp"

DebugBoard::DebugBoard(std::string url)
    : url_(url), ws_(easywsclient::WebSocket::from_url(url)) {}

DebugBoard::~DebugBoard() {
  ws_->close();
  printf("Closing WS\n");
}

void DebugBoard::poll() {
  if (ws_ == nullptr) {
    return;
  }
  ws_->poll(1);
  auto handler = [](const std::string &txt) {
    printf("Got text %s\n", txt.c_str());
  };
  ws_->dispatch(handler);
}
std::vector<Channel> DebugBoard::channels() { return {}; }
std::vector<Data> DebugBoard::updates() {
  auto tmp = incoming_;
  incoming_ = {};
  return tmp;
}

std::string DebugBoard::name() { return "Debug Board at " + url_; }