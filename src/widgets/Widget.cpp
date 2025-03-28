#include "Widget.hpp"
#include "imgui.h"
#include <format>
#include <map>

WidgetImpl::WidgetImpl(WidgetId id) : id_(id) {}

WidgetId WidgetImpl::Id() { return id_; }

void WidgetImpl::RegisterDataCallback(std::vector<DataLocator> wanted_data) {
  wanted_data_ = wanted_data;
}

std::vector<DataLocator> WidgetImpl::WantedData() const{ return wanted_data_; }
