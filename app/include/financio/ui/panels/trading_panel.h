#pragma once
#include "financio/rpc/trader_client.h"
#include "financio/ui/ui_manager.h"
#include "financio/ui/widgets/order_entry.h"
#include "financio/ui/widgets/symbol_selector.h"

namespace app {

class TradingPanel : public UIPanel {
public:
    TradingPanel(TraderClient* client);
    const char* name() const override { return "Trading"; }
    void on_draw() override;

private:
    TraderClient* m_client;
    SymbolSelector m_symbol;
    OrderEntry m_order;
};

} // namespace app
