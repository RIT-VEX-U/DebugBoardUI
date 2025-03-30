#include "datasources/DebugBoard.hpp"
#include "imgui.h"

using json = nlohmann::json;

DebugBoard::DebugBoard() {}

DebugBoard::~DebugBoard() {}

std::vector<DataUpdate> DebugBoard::PollData() {
  std::vector<DataUpdate> updates{};
  // return out list of unread stuff, reset internal list
  std::swap(updates, unread_updates);
  return updates;
}

DebugBoard::DataElementSet DebugBoard::ProvidedData() const {
  return current_channels;
}
void DebugBoard::feedPacket(const std::string &json_obj) {
  try {
    json data = json::parse(json_obj);
    feedPacket(data);
  } catch (const std::exception &e) {
    printf("Failed to parse json: %s\n", e.what());
  }
}

bool isAdvertise(const json &obj) {
  if (!obj.contains("type")) {
    return false;
  }
  return obj["type"] == "advertisement";
}

bool isData(const json &obj) {
  if (!obj.contains("type")) {
    return false;
  }
  return obj["type"] == "data";
}

void parseChannel(DataSource::DataElementSet &sofar, const json &chan_j) {

  if (!chan_j.contains("channel_id") || !chan_j.contains("schema")) {
    printf("Bad Channel\n");
    return;
  }
  if (!chan_j["channel_id"].is_number_integer()) {
    printf("Bad channel ud\n");
    return;
  }
  int chan_id = chan_j["channel_id"];

  std::function<void(const std::vector<std::string> &path_so_far,
                     const json &o)>
      walk;
  walk = [&](const std::vector<std::string> &path_so_far, const json &o) {
    if (!o.contains("type") || !o.contains("name")) {
      // don't know how to process
      printf("idk how to process\n");
      return;
    }
    if (!o["name"].is_string() || !o["type"].is_string()) {
      // bad types
      printf("bad types\n");
      return;
    }
    std::string name = o["name"];
    std::string valType = o["type"];
    std::vector<std::string> path = path_so_far;
    path.push_back(name);

    if (valType == "record") {
      if (!o.contains("fields") || !o["fields"].is_array()) {
        printf("bad record\n");
        return;
      } else {
        for (const json &field : o["fields"]) {
          walk(path, field);
        }
      }
    } else if (valType == "float") {
      sofar.insert(DataElementDescription{
          .path = DataPath{path}, .type_hint = DataPrimitiveType::Float});
    } else if (valType == "int") {
      sofar.insert(DataElementDescription{.path = DataPath{path},
                                          .type_hint = DataPrimitiveType::Int});
    } else if (valType == "uint") {
      sofar.insert(DataElementDescription{
          .path = DataPath{path}, .type_hint = DataPrimitiveType::Uint});
    } else if (valType == "string") {
      sofar.insert(DataElementDescription{
          .path = DataPath{path}, .type_hint = DataPrimitiveType::String});
    } else {
      printf("Unknown type: %s\n", valType.c_str());
    }
  };

  walk({std::to_string(chan_id)}, chan_j["schema"]);
}

void DebugBoard::HandleAdvertise(const json &json_obj) {
  std::string ms = json_obj.dump();
  printf("advertise: %s\n", ms.c_str());
  if (!json_obj.contains("channels") || !json_obj["channels"].is_array()) {
    return;
  }
  DataSource::DataElementSet open_streams{};
  for (const json &chan_j : json_obj["channels"]) {
    parseChannel(open_streams, chan_j);
  }
  current_channels = open_streams;
}

std::optional<DataError> DebugBoard::HandleData(const json &json_obj) {
  std::vector<DataElement> updates = {};

  // std::string ms = "outer" + json_obj.dump();
  // std::puts(ms.c_str());

  if (!json_obj.contains("channel_id") || !json_obj["channel_id"].is_number()) {
    return DataError{"Missing 'channel_id' or 'channel_id' wasn't number"};
  }
  if (!json_obj.contains("data") || !json_obj["data"].is_object()) {
    return DataError{"Missing 'data' or 'data' wasn't an object"};
  }
  size_t channel_id = json_obj["channel_id"];
  json data = json_obj["data"];

  for (const DataElementDescription &sup : current_channels) {
    if (sup.path.parts.size() < 1 ||
        sup.path.parts[0] != std::to_string(channel_id)) {
      continue;
    }

    const std::vector<std::string> &path = sup.path.parts;
    json curr_node = data;
    for (int path_idx = 1; path_idx < path.size(); path_idx++) {
      auto d = curr_node.dump();
      if (curr_node.contains(path[path_idx])) {
        curr_node = curr_node[path[path_idx]];
      } else {
        std::puts(std::format("Couldnt find {}", path[path_idx]).c_str());
        break;
      }
    }
    // if we got here, we have followed the thing to its end and havent skipped
    // curr_node is a data value

    DataLocator loc =
        DataLocator{.source_name = Name(), .path = sup.path, .special = false};
    if (sup.type_hint == DataPrimitiveType::Float) {
      if (curr_node.is_number()) {
        // we're good
        double value = curr_node;
        updates.push_back(DataElement{.path = loc, .value = value});

      } else {
        std::puts(curr_node.dump().c_str());
        printf("Expected double at but got something else\n");
      }

    } else if (sup.type_hint == DataPrimitiveType::Int) {
      printf("INT UNIMPLEMENTED" __FILE__ ":%d\n", __LINE__);
      return DataError{"INT Handler unimplemented"};
    } else if (sup.type_hint == DataPrimitiveType::Uint) {
      if (curr_node.is_number()) {
        // we're good
        size_t value = curr_node;
        updates.push_back(DataElement{.path = loc, .value = (uint64_t)value});

      } else {
        printf("Expected uint at but got something else\n");
        return DataError{std::format("Expected a Uint at '{}' but got {}",
                                     sup.path.toString(), curr_node.dump())};
      }
    } else {
      printf("INLKNOWN PRIITIZVE TYPE\n");
    }
  }

  unread_updates.push_back(
      DataUpdate{.rx_time = Timestamp::clock::now(), .new_data = updates});
  return {};
}

void DebugBoard::feedPacket(const json &json_obj) {
  if (isAdvertise(json_obj)) {
    HandleAdvertise(json_obj);
  } else if (isData(json_obj)) {
    auto res = HandleData(json_obj);
    if (res.has_value()) {
      std::puts(
          std::format("error feeding packet: {}", res.value().message).c_str());
    }
  } else {
    std::puts(std::format("Weird lookin packet: {}", json_obj.dump()).c_str());
  }
}

DebugBoardWebsocket::DebugBoardWebsocket(const std::string &ws_url,
                                         TimeDuration retry_period)
    : ws_url_(ws_url), ws_(easywsclient::WebSocket::from_url(ws_url)) {
  last_connect_time_ = std::chrono::steady_clock::now();
}

std::vector<DataUpdate> DebugBoardWebsocket::PollData() {
  if (ws_ == nullptr) {
    // TODO: maybe handle reconnect
    return DebugBoard::PollData();
  }

  ws_->poll();
  ws_->dispatch([&](std::string msg) { DebugBoard::feedPacket(msg); });

  return DebugBoard::PollData();
}

std::string DebugBoardWebsocket::Name() const { return "VDB@" + ws_url_; }

void DebugBoardWebsocket::Draw() {}