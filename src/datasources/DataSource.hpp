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

  virtual std::vector<Data> poll_updates() = 0;
};