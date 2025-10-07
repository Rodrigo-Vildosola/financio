#include "eng/runtime/application_base.h"

namespace eng {

void ApplicationBase::run() {
    Timer::init();
    const Timestep fixed_dt(1.0f/60.0f);
    f32 acc = 0.0f;
    while (m_running) {
        f32 now = Timer::elapsed();
        Timestep ts = now - m_last; m_last = now;
        acc += ts;

        while (acc >= fixed_dt) {
            for (Layer* l : m_stack) l->on_physics_update(fixed_dt);
            acc -= fixed_dt;
        }

        begin_frame();
        for (Layer* l : m_stack) l->on_update(ts);
        for (Layer* l : m_stack) l->on_ui_render(); // no-op unless a UI layer exists
        end_frame();

        pump_platform();
    }
}

}
