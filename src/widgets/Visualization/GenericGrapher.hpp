#include "../Types.hpp"
#include "GraphUtils.hpp"
#include "Widget.hpp"

#include <chrono>
#include <vector>
/**
 *
 */
class GenericGrapher final : public WidgetImpl {
public:
  struct AxisData {
    bool secondary_y = false;
    DataLocator loc = {};
    ScrollingBuffer<float> data;
  };
  struct SendingData {
    std::string data;
    DataLocator loc = {};
  };
  explicit GenericGrapher(WidgetId id);

  void ReceiveData(TimedData data) override;
  void Draw(bool *should_close) override;
  void ClearAndReregister();

private:
  float history;
  Timestamp creation_time_;
  DataLocator time_loc_;
  ScrollingBuffer<float> time_data_;
  char input_buffer[128] = "Input";
  std::vector<SendingData> input_data;
  std::vector<AxisData> data_;
  // ScrollingBuffer<float> time_;
};
