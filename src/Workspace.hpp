#pragma once
#include "Types.hpp"
#include "Widget.hpp"
namespace Workspace {
void Init();
void Draw();

void OpenWidget(Widget widg);

void AddSource(const std::shared_ptr<DataSource> &);
void RouteData();
} // namespace Workspace