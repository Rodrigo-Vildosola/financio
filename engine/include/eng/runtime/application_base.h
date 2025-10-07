#pragma once

#include "eng/core/base.h"
#include "eng/core/logger.h"
#include "eng/core/timestep.h"
#include "eng/core/layer_stack.h"

namespace eng {

class Layer;

struct CommandLineArgs {
    i32 count = 0;
    char** args = nullptr;

    const char* operator[](i32 index) const {
        ENG_CORE_ASSERT(index < count, "Index out of bounds for CommandLineArgs");
        return args[index];
    }
};


class ApplicationBase {
public:
    explicit ApplicationBase(CommandLineArgs args = {});
    virtual ~ApplicationBase();

    void run();
    void close();

    void push_layer(Layer* l);
    void push_overlay(Layer* l);

    CommandLineArgs args() const { 
        return m_args; 
    }

protected:
    virtual void begin_frame() {}
    virtual void end_frame() {}

    virtual void pump_platform() {} // signals, timers, IO progress

private:
    bool on_tick(Timestep dt);
    LayerStack m_stack;
    CommandLineArgs m_args;
    bool m_running = true;
    bool m_minimized = false;
    f32 m_last = 0.0f;
};



}
