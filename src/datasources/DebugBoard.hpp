#include "Types.hpp"
#include <cstdint>

#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "easywsclient/easywsclient.hpp"
#include <chrono>
#include <memory>
#include <string>
#include <vector>
/**
 * Defines how we interact with the debug board
 */
class DebugBoard : public DataSource {
public:
  DebugBoard();
  ~DebugBoard() override;

  void HandleAdvertise(const json &json_obj);
  std::optional<DataError> HandleData(const json &json_obj);
  
  json findChannelSchemaJson(int id);

  std::vector<DataUpdate> PollData() override;
  DataElementSet ProvidedData() const override;

  void feedPacket(const std::string &json_obj);
  void feedPacket(const json &json_obj);

private:
  DataElementSet current_channels;
  std::vector<json> current_channel_schemas;
  std::vector<DataUpdate> unread_updates;
};

/**
 * Defines how we interact with the debugboard's websocket
 */
class DebugBoardWebsocket : public DebugBoard {
public:
  using TimeDuration = std::chrono::duration<int64_t>;

  explicit DebugBoardWebsocket(const std::string &ws_url, TimeDuration retry_period = std::chrono::seconds(1));

  std::string Name() const override;
  std::vector<DataUpdate> PollData() override;
  std::string FormatSendingData(SendingData data_to_format);
  void SendData(SendingData data_to_send) override;
  void Draw() override;

private:
  TimeDuration retry_period;
  std::string ws_url_;
  std::unique_ptr<easywsclient::WebSocket> ws_;
  std::chrono::time_point<std::chrono::steady_clock> last_connect_time_;
};
