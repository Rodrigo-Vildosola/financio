// ui_manager.h
#pragma once

#include <imgui.h>
#include <vector>
#include <memory>

namespace app {

class UIPanel {
public:
    virtual ~UIPanel() = default;
    virtual const char* name() const = 0;
    virtual void on_draw() = 0;
};

class UIManager {
public:
    void register_panel(std::shared_ptr<UIPanel> panel);
    void draw();

private:
    std::vector<std::shared_ptr<UIPanel>> m_panels;
};

} // namespace app
