#pragma once

#include "eng/core/layer.h"
#include "eng/events/application_event.h"
#include "eng/events/key_event.h"
#include "eng/events/mouse_event.h"


namespace eng {

class RenderPass;

class UILayer : public Layer
{
public:
    UILayer();
    ~UILayer();

    virtual void on_attach() override;
    virtual void on_detach() override;
    virtual void on_event(Event& e) override;

    void begin();
    void end();
    void block_events(bool block) { m_block_events = block; }
    void set_dark_theme_colors();
private:
    bool m_block_events = true;
    f32 m_time = 0.0f;

    ref<RenderPass> m_ui_pass;
};

}
