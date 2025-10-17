#include "financio/ui/panels/account_panel.h"

#include <imgui.h>

namespace app {

void AccountPanel::on_draw() {
    ImGui::Text("Account Summary (placeholder)");
    ImGui::Separator();
    ImGui::Text("Balance: 0.00 USD");
    ImGui::Text("Equity:  0.00 USD");
    ImGui::Text("Margin:  0.00 USD");
}

} // namespace app
