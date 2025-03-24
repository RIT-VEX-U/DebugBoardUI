#include "Widget.hpp"
#include "imgui.h"
#include <format>
#include <map>

WidgetImpl::WidgetImpl(WidgetId id) : id_(id) {}

WidgetId WidgetImpl::Id() { return id_; }
