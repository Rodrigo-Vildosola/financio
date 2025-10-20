// ui_manager.cpp
#include "financio/ui/ui_manager.h"

namespace app {

void UIManager::register_panel(std::shared_ptr<UIPanel> panel) {
    m_panels.push_back(std::move(panel));
}

void UIManager::draw() {
    static bool dockspace_initialized = false;
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID);

    for (auto& panel : m_panels) {
        ImGui::Begin(panel->name());
        panel->on_draw();
        ImGui::End();
    }
}

} // namespace app
