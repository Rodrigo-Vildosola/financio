#include "financio/ui/panels/log_panel.h"
#include <imgui.h>

namespace app {

LogPanel::LogPanel(std::deque<std::string>* log)
    : m_log(log) {}

void LogPanel::on_draw() {
    ImGui::BeginChild("LogView", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
    for (auto& line : *m_log)
        ImGui::TextUnformatted(line.c_str());
    ImGui::EndChild();
}

} // namespace app
