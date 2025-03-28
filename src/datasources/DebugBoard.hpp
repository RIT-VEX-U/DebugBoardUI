#include "Types.hpp"
#include <cstdint>

#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "easywsclient/easywsclient.hpp"
#include <chrono>
#include <memory>
#include <string>
#include <vector>

class DebugBoard : public DataSource {
public:
  DebugBoard();
  ~DebugBoard();

  void HandleAdvertise(const json &json_obj);
  void HandleData(const json &json_obj);

  std::vector<DataUpdate> PollData() override;
  DataElementSet ProvidedData() const override;

  void feedPacket(const std::string &json_obj);
  void feedPacket(const json &json_obj);

private:
  DataElementSet current_channels;
  std::vector<DataUpdate> unread_updates;
};

class DebugBoardWebsocket : public DebugBoard {
public:
  using TimeDuration = std::chrono::duration<int64_t>;

  DebugBoardWebsocket(const std::string &ws_url,
                      TimeDuration retry_period = std::chrono::seconds(1));

  std::string Name() const override;
  std::vector<DataUpdate> PollData() override;
  void Draw() override;

private:
  std::string ws_url_;
  std::unique_ptr<easywsclient::WebSocket> ws_;
  std::chrono::time_point<std::chrono::steady_clock> last_connect_time_;
};
