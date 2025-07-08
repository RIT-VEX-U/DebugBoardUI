#pragma once
#include "../Types.hpp"
#include <functional>
#include <memory>
#include <string>

using WidgetId = std::size_t;

class WidgetImpl;
using Widget = std::shared_ptr<WidgetImpl>;
using DefaultWidgetCreator = std::function<Widget(WidgetId)>;

class WidgetImpl {
public:
  explicit WidgetImpl(WidgetId id);

  virtual void ReceiveData(TimedData data) = 0;
  // Use ImGui to draw a this widget
  virtual void Draw(bool *should_close) = 0;

  WidgetId Id() const;

  const DataLocationSet &WantedData() const;
  const SendingData &DataToSend() const;
  void ClearSentData();

protected:
  void RegisterDataCallback(const DataLocationSet &wanted_data);
  void SendToWebSocketCallback(const SendingData &data_to_send);

private:
  WidgetId id_;
  DataLocationSet wanted_data_;
  SendingData data_to_send_;
};
