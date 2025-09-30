#pragma once

#include "eng/core/base.h"
#include "eng/events/event.h"
#include "eng/core/timestep.h"


namespace eng {

class Layer {
public:
    Layer(const std::string& name = "Layer") : m_debug_name(name) {}
    virtual ~Layer() {}

    virtual void on_attach() {}
    virtual void on_detach() {}
    virtual void on_update(Timestep ts) {}
    virtual void on_physics_update(Timestep fixed_ts) {} // new!

    virtual void on_ui_render() {}
    virtual void on_event(Event& event) {}

    inline const std::string& get_name() const { return m_debug_name; }
protected:
    std::string m_debug_name;
};

}
