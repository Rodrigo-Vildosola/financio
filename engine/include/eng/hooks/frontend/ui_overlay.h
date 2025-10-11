#pragma once
#include <string>
#include "eng/events/event.h" // for Event
#include "eng/core/base.h"    // for f32, ref<>

namespace eng {

class RenderPass;
class GuiCtx;

struct UiOverlay {
    void on_attach(GuiCtx& c);
    void on_detach(GuiCtx& c);
    void begin(GuiCtx& c);
    void end(GuiCtx& c);
    void filter_event(Event& e);
private:
    void set_dark_theme_colors();
public:
    bool m_block_events = true;
    f32  m_time = 0.0f;
    ref<RenderPass> m_ui_pass;
};

} // namespace eng
