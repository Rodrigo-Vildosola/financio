#pragma once

#include "financio/ui/ui_manager.h"

#include <deque>
#include <string>

namespace app {

class LogPanel : public UIPanel {
public:
    explicit LogPanel(std::deque<std::string>* log);
    const char* name() const override { return "Log"; }
    void on_draw() override;

private:
    std::deque<std::string>* m_log;
};

} // namespace app
