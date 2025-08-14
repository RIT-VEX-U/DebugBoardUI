#include "../Types.hpp"
#include "GraphUtils.hpp"
#include "Widget.hpp"

#include <chrono>
#include <vector>
#include <array>
/**
 *
 */
class GenericGrapher final : public WidgetImpl {
public:
  //Struct for the axis data for the grapher
  struct AxisData {
    bool secondary_y = false;
    DataLocator loc = {};
    ScrollingBuffer<float> data;
  };

  /**
   * Creates a generic grapher with a widget id
   * @param id the widget id for the grapher to have
   */
  explicit GenericGrapher(WidgetId id);

  /**
   * Function for receiving data from the websocket
   * @param data the new data we have received
   */
  void ReceiveData(TimedData data) override;
  /**
   * Draws all the sections of the Generic Grapher
   * @param should_close whether or not we should close the grapher
   */
  void Draw(bool *should_close) override;
  /**
   * Function for Clearing and Reregistering the data in the grapher 
   */
  void ClearAndReregister();

private:
  std::vector<std::array<char, 128>> input_buffers;
  float history = 60;
  Timestamp creation_time_;
  DataLocator time_loc_;
  ScrollingBuffer<float> time_data_;
  std::vector<AxisData> data_;
};
