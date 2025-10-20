#pragma once

#include "financio/ui/ui_manager.h"

namespace app {

class AccountPanel : public UIPanel {
public:
    const char* name() const override { return "Account"; }
    void on_draw() override;
};

} // namespace app
