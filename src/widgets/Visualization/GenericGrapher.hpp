#include "GraphUtils.hpp"
#include "Widget.hpp"
#include <chrono>

class GenericGrapher : public WidgetImpl {
public:
  struct AxisData {
    bool secondary_y = false;
    DataLocator loc = {};
    ScrollingBuffer<float> data = ScrollingBuffer<float>{};
  };
  explicit GenericGrapher(WidgetId id);

  void ReceiveData(DataElement data) override;
  void Draw(bool *should_close) override;

private:
  std::chrono::seconds most_recent_time;
  DataLocator time_loc;
  std::vector<AxisData> data;
  ScrollingBuffer<float> time;
};