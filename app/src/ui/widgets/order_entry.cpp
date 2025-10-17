#include "financio/ui/widgets/order_entry.h"
#include <imgui.h>

namespace app {

void OrderEntry::on_draw() {
    ImGui::InputInt("Quantity", &m_quantity);
    ImGui::InputFloat("Limit Price", &m_price);
    if (ImGui::Button("Submit Order")) {
        // placeholder for order send
    }
}

} // namespace app
