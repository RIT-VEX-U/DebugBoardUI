#include "GraphUtils.hpp"
#include "Widget.hpp"
#include <chrono>
#include "../Types.hpp"
#include <vector>

class GenericGrapher final : public WidgetImpl {
public:
    struct Pt{
        float x;
        float y;
    };
  struct AxisData {
    bool secondary_y = false;
    DataLocator loc = {};
    ScrollingBuffer<Pt> data;
  };
  explicit GenericGrapher(WidgetId id);

  void ReceiveData(TimedData data) override;
  void Draw(bool *should_close) override;

private:

  Timestamp creation_time_;
  DataLocator time_loc_;
  std::vector<AxisData> data_;
  // ScrollingBuffer<float> time_;
};
