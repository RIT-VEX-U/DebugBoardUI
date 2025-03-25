#pragma once
#include "Channel.hpp"
#include <string>
#include <vector>

using Data = int;
class DataSource {
public:
  virtual ~DataSource() {}

  virtual std::vector<Channel> channels() = 0;
  virtual std::string name() = 0;

  virtual std::vector<Data> updates() = 0;
  virtual void poll() = 0;
};