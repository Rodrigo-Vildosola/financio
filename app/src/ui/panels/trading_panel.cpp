
#include "financio/ui/panels/trading_panel.h"

#include <imgui.h>

namespace app {

TradingPanel::TradingPanel(TraderClient* client)
    : m_client(client) {}

void TradingPanel::on_draw() {
    ImGui::Text("Trader client: %s", m_client ? "Ready" : "Null");
    ImGui::Separator();

    m_symbol.on_draw();
    m_order.on_draw();

    if (ImGui::Button("Connect")) {
        // placeholder
    }

    if (ImGui::Button("Subscribe")) {
        // placeholder
    }

    if (ImGui::Button("Disconnect")) {
        // placeholder
    }
}

} // namespace app
