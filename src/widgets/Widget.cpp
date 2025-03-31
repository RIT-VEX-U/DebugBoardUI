#include "Widget.hpp"
#include "../Types.hpp"
#include <vector>

WidgetImpl::WidgetImpl(WidgetId id) : id_(id) {}

WidgetId WidgetImpl::Id() const { return id_; }

void WidgetImpl::RegisterDataCallback(
    const DataLocationSet &wanted_data) {
  wanted_data_ = wanted_data;
}

const DataLocationSet &WidgetImpl::WantedData() const {
  return wanted_data_;
}
