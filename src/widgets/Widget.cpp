#include "Widget.hpp"
#include "../Types.hpp"
#include <vector>
#include <print>

WidgetImpl::WidgetImpl(WidgetId id) : id_(id) {}

WidgetId WidgetImpl::Id() const { return id_; }

void WidgetImpl::RegisterDataCallback(const DataLocationSet &wanted_data) {
  std::println("storing wanted data at the router");
  wanted_data_ = wanted_data;
}

void WidgetImpl::SendToWebSocketCallback(const SendingData &data_to_send) {
  data_to_send_ = data_to_send;
}

const DataLocationSet &WidgetImpl::WantedData() const {
  return wanted_data_;
}

SendingData &WidgetImpl::DataToSend() {
  return data_to_send_;
}