#include "eng/core/timer.h"
#include "eng/runtime/application_base.h"

namespace eng {

ApplicationBase::ApplicationBase(CommandLineArgs args) : m_command_line_args(args) {}

void ApplicationBase::run() {
    Timer::init();
    const Timestep fixed_dt(1.0f/60.0f);
    f32 acc = 0.0f;
    while (m_running) {
        f32 now = Timer::elapsed();
        Timestep ts = now - m_last_frame_time; m_last_frame_time = now;
        acc += ts;

        while (acc >= fixed_dt) {
            for (Layer* l : m_layer_stack) l->on_physics_update(fixed_dt);
            acc -= fixed_dt;
        }

        begin_frame();
        for (Layer* l : m_layer_stack) l->on_update(ts);
        for (Layer* l : m_layer_stack) l->on_ui_render(); // no-op unless a UI layer exists
        end_frame();

        pump_platform();
    }
}

void ApplicationBase::close() {
    m_running = false;
}

void ApplicationBase::push_layer(Layer* layer) {
    m_layer_stack.push_layer(layer);
    layer->on_attach();
}

void ApplicationBase::push_overlay(Layer* layer) {
    m_layer_stack.push_overlay(layer);
    layer->on_attach();
}

}
