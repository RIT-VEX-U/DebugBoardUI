#include "Types.hpp"
#include "imgui.h"
#include "implot/implot.h"

std::string DataPath::toString() const {
  if (path.size() == 0) {
    return "";
  }
  std::string accum = path[0];
  for (size_t i = 1; i < path.size(); i++) {
    accum += "/" + path[i];
  }
  return accum;
}
bool DataLocator::isEmpty() { return source_name.empty(); };
std::string DataLocator::toString() {
  if (isEmpty()) {
    return "(invalid location)";
  }
  return source_name + ":/" + path.toString();
}

bool DataLocator::operator==(const DataLocator &o) const {
  return o.path == path && o.source_name == source_name && o.special == special;
}
