#include "Widget.hpp"
#include "../Types.hpp"
#include <vector>

WidgetImpl::WidgetImpl(WidgetId id) : id_(id) {}

WidgetId WidgetImpl::Id() const { return id_; }

void WidgetImpl::RegisterDataCallback(const DataLocationSet &wanted_data) {
  wanted_data_ = wanted_data;
}

void WidgetImpl::SendToWebSocketCallback(const SendingData &data_to_send) {
  data_to_send_ = data_to_send;
}

const DataLocationSet &WidgetImpl::WantedData() const {
  return wanted_data_;
}

const SendingData &WidgetImpl::DataToSend() const {
  return data_to_send_;
}

void WidgetImpl::ClearSentData() {
  data_to_send_.data.clear();
  data_to_send_.loc.source_name.clear();
}