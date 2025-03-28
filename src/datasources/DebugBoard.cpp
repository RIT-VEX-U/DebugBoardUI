#include "datasources/DebugBoard.hpp"
#include "imgui.h"
#include <nlohmann/json.hpp>
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

  std::function<void(std::vector<std::string> path_so_far, const json &o)> walk;
  walk = [&](std::vector<std::string> path_so_far, const json &o) {
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
  printf("Walked:%d\n", (int)sofar.size());
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

void DebugBoard::HandleData(const json &json_obj) {
  std::vector<DataElement> updates = {};

  std::string ms = json_obj.dump();
  // printf("ms: %s\n", ms.c_str());

  if (!json_obj.contains("channel_id") ||
      !json_obj["channel_id"].is_number_unsigned()) {
    printf("no channel id for packet: %s\n", ms.c_str());
    return;
  }
  if (!json_obj.contains("data") || !json_obj["data"].is_object()) {
    printf("Wrong type of data\n");
    return;
  }
  size_t channel_id = json_obj["channel_id"];
  json data = json_obj["data"];

  std::string du = data.dump();
  // printf("DU: %s\n", du.c_str());

  for (const DataElementDescription &sup : current_channels) {
    if (sup.path.path.size() < 1 ||
        sup.path.path[0] != std::to_string(channel_id)) {
      printf("Skipping bc wrong channel\n");
      continue;
    }
    // valid thingy
    std::string s = sup.path.toString();
    // printf("Checking %s\n", s.c_str());
    const std::vector<std::string> &path = sup.path.path;
    json curr_node = data;
    for (int path_idx = 1; path_idx < path.size(); path_idx++) {
      auto d = curr_node.dump();
      // printf("looking at %s\n", d.c_str());
      if (curr_node.contains(path[path_idx])) {
        curr_node = curr_node[path[path_idx]];
        // printf("Following: %s\n", path[path_idx].c_str());
      } else {
        printf("Couldnt find %s\n", path[path_idx].c_str());
        break;
      }
    }
    // if we got here, we have followed the thing to its end and havent skipped
    // curr_node is a data value
    // printf("Found: %s of %s\n", curr_node.dump().c_str(),
    //  curr_node.type_name());
    DataLocator loc =
        DataLocator{.source_name = Name(), .path = sup.path, .special = false};
    if (sup.type_hint == DataPrimitiveType::Float) {
      if (curr_node.is_number()) {
        // we're good
        double value = curr_node;
        updates.push_back(DataElement{.path = loc, .value = value});

      } else {
        printf("Expected double at but got something else\n");
      }

    } else if (sup.type_hint == DataPrimitiveType::Int) {
      printf("INT UNIMPLEMENTED" __FILE__ ":%d\n", __LINE__);
    } else if (sup.type_hint == DataPrimitiveType::Uint) {
      if (curr_node.is_number()) {
        // we're good
        size_t value = curr_node;
        updates.push_back(DataElement{.path = loc, .value = value});

      } else {
        printf("Expected uint at but got something else\n");
      }
    } else {
      printf("INLKNOWN PRIITIZVE TYPE\n");
    }
  }

  unread_updates.push_back(
      DataUpdate{.rx_time = Timestamp::clock::now(), .new_data = updates});
}

void DebugBoard::feedPacket(const json &json_obj) {
  if (isAdvertise(json_obj)) {
    HandleAdvertise(json_obj);
  } else {
    HandleData(json_obj);
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
  ws_->dispatch([&](std::string msg) {
    // printf("Got: %s\n", msg.c_str());
    DebugBoard::feedPacket(msg);
  });

  return DebugBoard::PollData();
}

std::string DebugBoardWebsocket::Name() const { return "VDB@" + ws_url_; }

void DebugBoardWebsocket::Draw() {}