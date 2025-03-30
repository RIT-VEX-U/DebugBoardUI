#pragma once
#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_set>
#include <variant>
#include <vector>

/**
 * DataPath - a comparable representation of a position in a hierarchy
 */
struct DataPath {
  /**
   * Convert path to a string for printing using / as a separator
   * @return string representation
   */
  std::string toString() const;
  /**
   * Check if two paths are equal. Does string comparison on all elements
   * @param o object to compare to
   * @return true if the paths are equal
   */
  bool operator==(const DataPath &o) const;

  std::vector<std::string> parts;
};
// Hasher for DataPath. Allows use in sets
struct DataPathHasher {
  size_t operator()(const DataPath &path) const;
};

struct DataLocator {
  std::string source_name;
  DataPath path;
  bool special;

  std::string toString();
  bool isEmpty();
  bool operator==(const DataLocator &) const;
};

/**
 * Types that can be made available by sources.
 * DataPrimitiveType only identifies them. DataPrimitive can be used to hold
 * these types
 */
enum DataPrimitiveType {
  String,
  Float,
  Int,
  Uint,
};

/**
 * Type that can hold base element of data that sources can provide
 * "The thing at the end of the path"
 */
using DataPrimitive = std::variant<std::string, double, int64_t, uint64_t>;

struct DataElementDescription {
  DataPath path;
  DataPrimitiveType type_hint;
  bool operator==(const DataElementDescription &o) const;
};

struct DataElement {
  DataLocator path;
  DataPrimitive value;
};

using Timestamp = std::chrono::time_point<std::chrono::steady_clock>;

struct DataUpdate {
  Timestamp rx_time;
  std::vector<DataElement> new_data;
};

struct DataElementDescriptionHash;
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

/**
 * Draw a widget used for selecting data sources
 * presents a menu showing available datasources and their data channels
 * @param[in] name a name to display for this input
 * @param[inout] current the current locator, when a user selects a new choice,
 * this will be reflected to use that choice
 * @return true if the user made a selection with this widget
 */
bool DataLocationSelector(const char *name, DataLocator &current);

struct DataElementDescriptionHash {
  size_t operator()(const DataElementDescription &obj) const;
};
