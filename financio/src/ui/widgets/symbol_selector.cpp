#include "financio/ui/widgets/symbol_selector.h"

#include <imgui.h>

namespace app {

void SymbolSelector::on_draw() {
    ImGui::InputText("Symbol", m_symbol.data(), m_symbol.capacity() + 1);
}

} // namespace app
