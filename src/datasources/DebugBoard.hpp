#include "datasources/DataSource.hpp"
#include <cstdint>

#include "easywsclient/easywsclient.hpp"
#include <memory>
#include <string>
#include <vector>
class DebugBoard : public DataSource {
public:
  DebugBoard(std::string url);
  ~DebugBoard();
  void poll() override;
  std::vector<Channel> channels() override;
  std::vector<Data> updates() override;
  std::string name() override;

private:
  std::string url_;
  std::unique_ptr<easywsclient::WebSocket> ws_;
  std::vector<Data> incoming_;
};