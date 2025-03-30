#include "Types.hpp"
#include <cstddef>
#include <functional>
#include <string>

std::string DataPath::toString() const {
  if (parts.empty()) {
    return "";
  }
  std::string accum = parts[0];
  for (size_t i = 1; i < parts.size(); i++) {
    accum += "/" + parts[i];
  }
  return accum;
}
bool DataLocator::isEmpty() const { return source_name.empty(); };
std::string DataLocator::toString() const {
  if (isEmpty()) {
    return "(invalid location)";
  }
  return source_name + ":/" + path.toString();
}

bool DataLocator::operator==(const DataLocator &other) const {
  return other.path == path && other.source_name == source_name && other.special == special;
}

bool DataPath::operator==(const DataPath &other) const {
  if (other.parts.size() != parts.size()) {
    return false;
  }
  for (size_t i = 0; i < parts.size(); i++) {
    if (parts[i] != other.parts[i]) {
      return false;
    }
  }
  return true;
}

bool DataElementDescription::operator==(const DataElementDescription &other) const {
  return path == other.path && type_hint == other.type_hint;
}

size_t DataElementDescriptionHash::operator()(const DataElementDescription &obj) const
{
    const size_t h1 = DataPathHasher{}(obj.path);
    const size_t h2 = std::hash<DataPrimitiveType>{}(obj.type_hint);
    return h1 ^ (h2 << 1); // Combine hash values
}

size_t DataPathHasher::operator()(const DataPath &path) const {
  size_t combined_hash = 0;
  for (const std::string &str : path.parts) {
      std::hash<std::string> const hasher;
      const size_t string_hash = hasher(str);
      combined_hash ^= string_hash + 0x9e3779b9 + (combined_hash << 6) + (combined_hash >> 2);
  }
  return combined_hash;
}
