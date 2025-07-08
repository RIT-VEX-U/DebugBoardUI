#include "datasources/DebugBoard.hpp"
#include "Types.hpp"
#include "easywsclient/easywsclient.hpp"
#include "imgui.h"
#include "nlohmann/json_fwd.hpp"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <exception>
#include <format>
#include <functional>
#include <optional>
#include <print>
#include <string>
#include <utility>
#include <vector>
#include <chrono>
#include <thread>

using json = nlohmann::json;

DebugBoard::DebugBoard() = default;

DebugBoard::~DebugBoard() = default;

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
    json const data = json::parse(json_obj);
    feedPacket(data);
  } catch (const std::exception &e) {
    std::println("Failed to parse json: {}", e.what());
  }
}

static bool isAdvertise(const json &obj) {
  if (!obj.contains("type")) {
    return false;
  }
  return obj["type"] == "advertisement";
}

static bool isData(const json &obj) {
  if (!obj.contains("type")) {
    return false;
  }
  return obj["type"] == "data";
}
/**
 * Parses Channels from the advertisement
 * @param sofar the data element set of the advertisement so far
 * @param chan_j the json of the channel from the advertisement
 */
static void parseChannel(DataSource::DataElementSet &sofar, const json &chan_json) {
  println("Channel to Parse: {}", chan_json.dump());
  //checks that it contains a channel id and channel schema
  if (!chan_json.contains("channel_id") || !chan_json.contains("schema")) {
    std::puts("Bad Channel");
    return;
  }
  //checks that the channel id is a number
  if (!chan_json["channel_id"].is_number_integer()) {
    std::puts("Bad channel id");
    return;
  }
  //gets the channel id
  int const chan_id = chan_json["channel_id"];

//walks through the list of channels and processes each one
  std::function<void(const std::vector<std::string> &path_so_far, const json &o)>walk;
  walk = [&](const std::vector<std::string> &path_so_far, const json &o) {
    //channels should have a type and a name
    if (!o.contains("type") || !o.contains("name")) {
      // don't know how to process
      std::println("idk how to process");
      return;
    }
    //channel types and names should be strings
    if (!o["name"].is_string() || !o["type"].is_string()) {
      // bad types
      std::println("bad types");
      return;
    }
    //gets the name and type of the channel
    std::string const name = o["name"];
    std::string valType = o["type"];
    println("parsed name: {}, of type: {}", name, valType);
    //gets the path of the name
    std::vector<std::string> path = path_so_far;
    
    path.push_back(name);
    println("path parsed: {}", path);

    //handles the data accordingly to the type
    if (valType == "record") {
      //handles a record by walking through each field and handling each feild
      if (!o.contains("fields") || !o["fields"].is_array()) {
        std::println("bad record\n");
        return;
      } else {
        for (const json &field : o["fields"]) {
          walk(path, field);
        }
      }
      //handles other types by creating a data element description from the items path, and type
    } else if (valType == "float") { 
      sofar.insert(DataElementDescription{.path = DataPath{path}, .type_hint = DataPrimitiveType::Float});
    } else if (valType == "int") {
      sofar.insert(DataElementDescription{.path = DataPath{path}, .type_hint = DataPrimitiveType::Int});
    } else if (valType == "uint") {
      sofar.insert(DataElementDescription{.path = DataPath{path}, .type_hint = DataPrimitiveType::Uint});
    } else if (valType == "string") {
      sofar.insert(DataElementDescription{.path = DataPath{path}, .type_hint = DataPrimitiveType::String});
    } else {
      std::println("Unknown type: %s", valType);
    }
  };

  walk({std::to_string(chan_id)}, chan_json["schema"]);
}

/**
 * Handles Advertisement messages from the websocket
 * @param json_obj the json object of the advertisement message
 */
void DebugBoard::HandleAdvertise(const json &json_obj) {
  std::println("advertisement receieved from websocket: {}", json_obj.dump());
  //checks if the message contains a list of channels, which it needs to have if it is an advertisement
  if (!json_obj.contains("channels") || !json_obj["channels"].is_array()) {
    return;
  }
  //creates a set of data elements and jsons from the channels advertised
  DataSource::DataElementSet open_streams{};
  current_channel_schemas.clear();
  for (const json &chan_j : json_obj["channels"]) {
    parseChannel(open_streams, chan_j);
    current_channel_schemas.push_back(chan_j);
  }
  //sets the current channels it knows about
  current_channels = open_streams;
}
/**
 * Handles Data messages from the websocket
 * @param json_obj the json object of the data message
 */
std::optional<DataError> DebugBoard::HandleData(const json &json_obj) {
  std::println("data recieved from websocket: {}", json_obj.dump());
  //the updates to data that we should end with after handling the data
  std::vector<DataElement> updates = {};

  //checks if the message contains data and a channel id which it needs to be a data message
  if (!json_obj.contains("channel_id") || !json_obj["channel_id"].is_number()) {
    return DataError{"Missing 'channel_id' or 'channel_id' wasn't number"};
  }
  if (!json_obj.contains("data") || !json_obj["data"].is_object()) {
    return DataError{"Missing 'data' or 'data' wasn't an object"};
  }
  //sets the channel id to a size and the data to its own json objectA
  size_t const channel_id = json_obj["channel_id"];
  const json data = json_obj["data"];

  //look through each data element description in our list of advertised channels
  for (const DataElementDescription &datadesc : current_channels) {
    //skip over empty parts and the channel id which we already have
    if (datadesc.path.parts.empty() || datadesc.path.parts[0] != std::to_string(channel_id)) {
      continue;
    }
    //get the path of the data we are looking for
    const std::vector<std::string> &path = datadesc.path.parts;
    // println("data description path: {}", datadesc.path.toString());
    //set the current node to json node of the data we have received
    json curr_node = data;
    //look through the path for the data we were advertised in the data we actually received
    for (int path_idx = 1; path_idx < path.size(); path_idx++) {
      auto d = curr_node.dump();
      if (curr_node.contains(path[path_idx])) {
        std::println("found {} in {}", path[path_idx], d);
        curr_node = curr_node[path[path_idx]];
      } else {
        std::println("Couldnt find {} in {}", path[path_idx], d);
        break;
      }
    }
    // if we got here, we have followed the thing to its end and havent skipped
    // curr_node is a data value

    DataLocator const loc = DataLocator{
        .source_name = Name(), 
        .path = datadesc.path, 
        .is_rx_time = false};
    if (datadesc.type_hint == DataPrimitiveType::Float) {
      if (curr_node.is_number()) {
        // we're good
        double value = curr_node;
        updates.push_back(DataElement{.location = loc, .value = value});

      } else {
        std::println("Expected double at {} but got something else", curr_node.dump());
      }

    } else if (datadesc.type_hint == DataPrimitiveType::Int) {
      std::println("INT UNIMPLEMENTED {}:{}", __FILE__, __LINE__);
      return DataError{"INT Handler unimplemented"};
    } else if (datadesc.type_hint == DataPrimitiveType::Uint) {
      if (curr_node.is_number()) {
        // we're good
        size_t value = curr_node;
        updates.push_back(
            DataElement{.location = loc, .value = (uint64_t)value});

      } else {
        std::println("Expected uint at but got something else\n");
        return DataError{std::format("Expected a Uint at '{}' but got {}", datadesc.path.toString(), curr_node.dump())};
      }
    } else {
      std::println("INLKNOWN PRIITIZVE TYPE\n");
    }
  }

  unread_updates.push_back(
      DataUpdate{.rx_time = Timestamp::clock::now(), .new_data = updates});
  return {};
}

json DebugBoard::findChannelSchemaJson(int id){
  for(json channel: current_channel_schemas){
    if(channel["channel_id"] == id){
      return channel["schema"];
    }
  }
  printf("couldn't find channel with id: %d", id);
  return nullptr;
}

void DebugBoard::feedPacket(const json &json_obj) {
  if (isAdvertise(json_obj)) {
    HandleAdvertise(json_obj);
  } else if (isData(json_obj)) {
    auto res = HandleData(json_obj);
    if (res.has_value()) {
      std::println("error feeding packet: {}", res.value().message);
    }
  } else {
    std::println("Weird lookin packet: {}", json_obj.dump());
  }
}

DebugBoardWebsocket::DebugBoardWebsocket(const std::string &ws_url, TimeDuration retry_period): 
ws_url_(ws_url), ws_(easywsclient::WebSocket::from_url(ws_url)) {
  last_connect_time_ = std::chrono::steady_clock::now();
  this->retry_period = retry_period;
}

std::vector<DataUpdate> DebugBoardWebsocket::PollData() {
  if (ws_ == nullptr) {
    //idk if this works, looks like it dont
    printf("Failed to connect to websocket, retrying in %lld seconds\n", retry_period.count());
    std::this_thread::sleep_for(retry_period);
    this->PollData();
    // TODO: maybe handle reconnect
    return DebugBoard::PollData();
  }

  ws_->poll();
  ws_->dispatch([&](const std::string &msg) { DebugBoard::feedPacket(msg); });

  return DebugBoard::PollData();
}

std::string DebugBoardWebsocket::FormatSendingData(SendingData data_to_format){
  const json &channel_schema = DebugBoard::findChannelSchemaJson(stoi(data_to_format.loc.path.parts[0]));
  // printf("channel schema found: \n%s\n\n", nlohmann::to_string(channel_schema).c_str());

  nlohmann::ordered_json data_schema = json::object();
  std::function<void(nlohmann::ordered_json &current_node, const json &o)>walk_schema;
  walk_schema = [&](nlohmann::ordered_json &current_node, const json &o) {
    //channels should have a type and a name
    if (!o.contains("type") || !o.contains("name")) {
      // don't know how to process
      std::println("idk how to process");
      return;
    }
    //channel types and names should be strings
    if (!o["name"].is_string() || !o["type"].is_string()) {
      // bad types
      std::println("bad types");
      return;
    }
    

    //gets the name and type of the channel
    // println("schema so far : {}\n", nlohmann::to_string(data_schema));
    std::string const str_name = o["name"];
    std::string valType = o["type"];
    
    //gets the path of the name
    // println("current node before insertion: {}\n", nlohmann::to_string(current_node));
    nlohmann::ordered_json &next_node = current_node[str_name];
    // println("next node: {}\n", nlohmann::to_string(next_node));
    //handles the data accordingly to the type
    if (valType == "record") {
      //handles a record by walking through each field and handling each feild
      if (!o.contains("fields") || !o["fields"].is_array()) {
        // std::println("bad record\n");
        return;
      } else {
        for (const json &field : o["fields"]) {
          // std::println("entering node: {}\n", nlohmann::to_string(next_node));
          walk_schema(next_node, field);
        }

      }
    }
    else if(str_name == data_to_format.loc.path.parts[data_to_format.loc.path.parts.size()-1]){
      next_node = data_to_format.data;
    }
    else{
      next_node = "N/A";
    }
    // println("current node after insertion: {}\n", nlohmann::to_string(current_node));
  };
  
  walk_schema(data_schema, channel_schema);
  // println("schema json: \n{}\n\n", nlohmann::to_string(data_schema));

  nlohmann::ordered_json full_data_json = {
    {"type", "data"},
    {"channel_id", data_to_format.loc.path.parts[0]},
    {"name", data_to_format.loc.path.parts[1]},
    {"data", data_schema}
  };
  printf("created full data json: \n%s\n\n", nlohmann::to_string(full_data_json).c_str());
  


  return nlohmann::to_string(full_data_json);
}
/**
 * sends the SendingData to the debug board's websocket
 * @param data_to_send the data to send to the websocket
 */
void DebugBoardWebsocket::SendData(SendingData data_to_send) {

  std::string data_json_string = this->FormatSendingData(data_to_send);
  printf("sending data to the websocket\n");
  ws_->send(data_json_string);
  ws_->poll();
}

std::string DebugBoardWebsocket::Name() const { return "VDB@" + ws_url_; }

void DebugBoardWebsocket::Draw() {
  if (ImGui::Begin(std::format("VDB @ {}", ws_url_).c_str())) {
    ImGui::Text("asdf\n"); 
  }
  ImGui::End();
}
