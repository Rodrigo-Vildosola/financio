#pragma once
#include "financio/ui/ui_manager.h"

namespace app {

class ChartPanel : public UIPanel {
public:
    const char* name() const override { return "Chart"; }
    void on_draw() override;
};

} // namespace app
