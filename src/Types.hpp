#pragma once
#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_set>
#include <variant>
#include <vector>

struct DataPath {
  std::vector<std::string> path;
  std::string toString();

  bool operator==(const DataPath &o) const {
    if (o.path.size() != path.size()) {
      return false;
    }
    for (size_t i = 0; i < path.size(); i++) {
      if (path[i] != o.path[i]) {
        return false;
      }
    }
    return true;
  }
};
struct DataPathHasher {
  size_t operator()(const DataPath &path) const {
    size_t combined_hash = 0;
    for (const std::string &str : path.path) {
      std::hash<std::string> hasher;
      size_t string_hash = hasher(str);
      combined_hash ^= string_hash + 0x9e3779b9 + (combined_hash << 6) +
                       (combined_hash >> 2);
    }
    return combined_hash;
  }
};

struct DataLocator {
  std::string source_name;
  DataPath path;
  bool special;

  std::string toString();
  bool isEmpty();
};

enum DataPrimitiveType {
  String,
  Double,
  Int,
  Uint,
};
using DataPrimitive = std::variant<std::string, double, int64_t, uint64_t>;

struct DataElementDescription {
  DataPath path;
  DataPrimitiveType type_hint;
  bool operator==(const DataElementDescription &o) const {
    return path == o.path && type_hint == o.type_hint;
  }
};

struct DataElement {
  DataPath path;
  DataPrimitive value;
};

using Timestamp = std::chrono::time_point<std::chrono::steady_clock>;

struct DataUpdate {
  Timestamp rx_time;
  std::vector<DataElement> new_data;
};
struct DataElementDescriptionHash {
  size_t operator()(const DataElementDescription &obj) const {
    size_t h1 = DataPathHasher{}(obj.path);
    size_t h2 = std::hash<DataPrimitiveType>{}(obj.type_hint);
    return h1 ^ (h2 << 1); // Combine hash values
  }
};

class DataSource {
public:
  using DataElementSet =
      std::unordered_set<DataElementDescription, DataElementDescriptionHash>;
  virtual ~DataSource() {}
  /// a human readable name describing this data source
  virtual std::string Name() const = 0;
  virtual DataElementSet ProvidedData() const = 0;
  virtual std::vector<DataUpdate> PollData() = 0;

  virtual void Draw() = 0;
};

bool DataLocationSelector(const char *name, DataLocator &current);