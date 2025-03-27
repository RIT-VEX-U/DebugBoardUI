#include "imgui.h"

#include "platform/glue.hpp"
#include <math.h>

#include "Workspace.hpp"
#include "datasources/DebugBoard.hpp"
#include "nlohmann/json.hpp"
#include "widgets/WidgetRegistry.hpp"
#include <cstdio>
#include <format>
#include <string>

class DumbDataSource : public DataSource {
public:
  DumbDataSource() noexcept {}
  ~DumbDataSource() {}
  /// a human readable name describing this data source
  std::string Name() const override { return "dumb source 1"; }
  ProvidedDataT ProvidedData() const override {
    ProvidedDataT dats;
    dats.insert(DataElementDescription{.path = {{"Voltage"}},
                                       .type_hint = DataPrimitiveType::Double});
    dats.insert(DataElementDescription{.path = {{"Current"}},
                                       .type_hint = DataPrimitiveType::Double});
    return dats;
  }
  std::vector<DataUpdate> PollData() override { return {}; };

  void Draw() override {
    ImGui::Begin("Dumb source 1");
    ImGui::Text("hellooo");
    ImGui::End();
  }
};

int main() {
  DebugBoard board{"ws://localhost:8080/ws"};
  Workspace::Init();
  Workspace::AddSource(std::make_shared<DumbDataSource>());

  Platform::Data pdata = Platform::init("Debug Board UI");

  // Main loop
  while (!Platform::shouldclose(pdata)) {
    board.PollData();

    Platform::prerender(pdata);

    Workspace::Draw();

    Platform::postrender(pdata);
  }

  Platform::cleanup(pdata);
}