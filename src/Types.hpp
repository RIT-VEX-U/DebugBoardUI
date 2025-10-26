#pragma once
#include <chrono>
#include <cstdint>
#include <expected>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

struct DataError {
  std::string message;
};

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
  bool is_rx_time;

  std::string toString() const;
  bool isEmpty() const;
  bool operator==(const DataLocator &) const;
};
// Hasher for DataLocator. Allows use in sets
struct DataLocatorHasher {
  size_t operator()(const DataLocator &) const;
};

using DataLocationSet = std::unordered_set<DataLocator, DataLocatorHasher>;

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
  Bool,
};

/**
 * Type that can hold base element of data that sources can provide
 * "The thing at the end of the path"
 */
using DataPrimitive = std::variant<std::string, double, bool, int64_t, uint64_t>;

struct DataElementDescription {
  DataPath path;
  DataPrimitiveType type_hint;
  bool operator==(const DataElementDescription &o) const;
};

// Allows use of DataElementDescription in sets
struct DataElementDescriptionHash {
  size_t operator()(const DataElementDescription &obj) const;
};

struct DataElement {
  DataLocator location;
  DataPrimitive value;
  DataPrimitiveType type;
};

//Struct for data we want to send that we got from the grapher
struct SendingData {
  bool waiting_to_send = false;
  DataPrimitive data;
  DataPrimitiveType data_type;
  DataLocator loc = {};
};

template <> struct std::formatter<DataLocator> {
  constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }
};

template <> struct std::formatter<DataPrimitive> {
  constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

  auto format(const DataPrimitive &obj, std::format_context &ctx) const {
    return std::format_to(ctx.out(), "Value");
    // std::visit<>([&ctx](const auto &
    // val) { return std::format_to(ctx.out(), "Value: {}", val); },
    // obj);
  }
};

template <> struct std::formatter<DataElement> {
  constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

  auto format(const DataElement &obj, std::format_context &ctx) const {
    return std::format_to(ctx.out(), "ASDF");
    // std::format_to(ctx.out(), "Location: {}, Value: {}", obj.location,
    // obj.value);
  }
};

using Timestamp = std::chrono::time_point<std::chrono::steady_clock>;

struct DataAndTime {
  DataPrimitive value;
  DataPrimitiveType type;
  Timestamp time;
};


template <> struct std::formatter<DataAndTime> {
  constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

  auto format(const DataAndTime &obj, std::format_context &ctx) const {
    return std::format_to(ctx.out(), "DataAndTime{{ {} }}", obj.value);
  }
};

using TimedData =
    std::unordered_map<DataLocator, DataAndTime, DataLocatorHasher>;

struct DataUpdate {
  Timestamp rx_time;
  std::vector<DataElement> new_data;
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
  virtual void SendData(SendingData &data_to_send) = 0;
  virtual bool isReady() = 0;

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

enum class DataRetrieveFailure {
  KeyNotFound,
  ValueWrongType,
};
std::expected<double, DataRetrieveFailure> getDoubleAt(const DataLocator &loc,
                                                       const TimedData &data);
