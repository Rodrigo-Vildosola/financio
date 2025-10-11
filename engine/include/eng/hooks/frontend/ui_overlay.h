#pragma once
#include <string>
#include "eng/core/context.h"
#include "eng/core/caps.h"
#include "eng/core/layer/layer.h"
#include "eng/core/application.h"

namespace eng {

class RenderPass;

struct UiOverlay {
    template<class Ctx> 
    void on_attach(Ctx& c);

    template<class Ctx> 
    void on_detach(Ctx& c);

    template<class Ctx> 
    void begin(Ctx& c);

    template<class Ctx> 
    void end(Ctx& c);

    void filter_event(Event& e);
private:
    void set_dark_theme_colors();

public:
    bool m_block_events = true;
    f32  m_time = 0.0f;
    ref<RenderPass> m_ui_pass;
};
} // namespace eng
