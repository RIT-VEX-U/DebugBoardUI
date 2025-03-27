#include "Types.hpp"
#include <cstdint>

#include "easywsclient/easywsclient.hpp"
#include <memory>
#include <string>
#include <vector>
class DebugBoard : public DataSource {
public:
  DebugBoard(std::string url);
  ~DebugBoard();
  std::string Name() const override;
  std::vector<DataUpdate> PollData() override;
  DebugBoard::ProvidedDataT ProvidedData() const override;
  void Draw() override;

private:
  std::string url_;
  std::unique_ptr<easywsclient::WebSocket> ws_;
};