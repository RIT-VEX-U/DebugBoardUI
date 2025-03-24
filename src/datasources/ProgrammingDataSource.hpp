#pragma once
#include "datasources/DataSource.hpp"

class ProgrammingDataSource : public DataSource {
public:
  ProgrammingDataSource();
  std::string name() override;

  void Draw();
};