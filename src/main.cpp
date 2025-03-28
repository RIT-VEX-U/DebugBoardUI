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
  DumbDataSource() noexcept {
    dats.insert(DataElementDescription{.path = {{"Voltage"}},
                                       .type_hint = DataPrimitiveType::Float});
    dats.insert(DataElementDescription{.path = {{"Current"}},
                                       .type_hint = DataPrimitiveType::Float});
  }
  ~DumbDataSource() {}
  /// a human readable name describing this data source
  std::string Name() const override { return "dumb source 1"; }
  DataElementSet ProvidedData() const override { return dats; }
  std::vector<DataUpdate> PollData() override { return {}; };

  void Draw() override {
    ImGui::Begin("Dumb source 1");
    ImGui::Text("hellooo");
    ImGui::End();
  }

private:
  DataElementSet dats;
};

int main() {
  // auto board =
  // std::make_shared<DebugBoardWebsocket>("ws://localhost:8080/ws");
  auto board = std::make_shared<DebugBoardWebsocket>("ws://129.21.144.84/ws");
  Workspace::Init();
  Workspace::AddSource(board);

  Platform::Data pdata = Platform::init("Debug Board UI");

  // Main loop
  while (!Platform::shouldclose(pdata)) {
    board->PollData();

    Platform::prerender(pdata);

    Workspace::Draw();

    Platform::postrender(pdata);
  }

  Platform::cleanup(pdata);
}