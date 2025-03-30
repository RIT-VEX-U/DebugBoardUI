#include "Types.hpp"
#include "imgui.h"
#include "implot/implot.h"

std::string DataPath::toString() const {
  if (parts.size() == 0) {
    return "";
  }
  std::string accum = parts[0];
  for (size_t i = 1; i < parts.size(); i++) {
    accum += "/" + parts[i];
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

bool DataPath::operator==(const DataPath &o) const {
  if (o.parts.size() != parts.size()) {
    return false;
  }
  for (size_t i = 0; i < parts.size(); i++) {
    if (parts[i] != o.parts[i]) {
      return false;
    }
  }
  return true;
}

bool DataElementDescription::operator==(const DataElementDescription &o) const {
  return path == o.path && type_hint == o.type_hint;
}

size_t DataElementDescriptionHash::operator()(
    const DataElementDescription &obj) const {
  size_t h1 = DataPathHasher{}(obj.path);
  size_t h2 = std::hash<DataPrimitiveType>{}(obj.type_hint);
  return h1 ^ (h2 << 1); // Combine hash values
}


size_t DataPathHasher::operator()(const DataPath &path) const {
  size_t combined_hash = 0;
  for (const std::string &str : path.parts) {
    std::hash<std::string> hasher;
    size_t string_hash = hasher(str);
    combined_hash ^=
        string_hash + 0x9e3779b9 + (combined_hash << 6) + (combined_hash >> 2);
  }
  return combined_hash;
}