#pragma once

#include "eng/core/application.h"

namespace eng {

template<class AppCfg, class Ctx, class Hooks>
Application<AppCfg, Ctx, Hooks>::Application(CommandLineArgs args) : m_args(args), m_stack(&m_ctx) {
    Hooks::init(m_ctx, &m_stack); // pass stack so hooks can forward events
}

template<class AppCfg, class Ctx, class Hooks>
Application<AppCfg, Ctx, Hooks>::~Application() {
    Hooks::shutdown(m_ctx);
}

template<class AppCfg, class Ctx, class Hooks>
template<class L, class... Args>
L& Application<AppCfg, Ctx, Hooks>::push_root_layer(Args&&... a) {
    return m_stack.template push_layer<AppCfg, L>(std::forward<Args>(a)...);
}

template<class AppCfg, class Ctx, class Hooks>
template<class L, class... Args>
L& Application<AppCfg, Ctx, Hooks>::push_overlay(Args&&... a) {
    return m_stack.template push_overlay<AppCfg, L>(std::forward<Args>(a)...);
}

template<class AppCfg, class Ctx, class Hooks>
void Application<AppCfg, Ctx, Hooks>::run() {

    Timer::init();

    const Timestep fixed_dt(1.0f/60.0f);
    f32 acc = 0.f;
    f32 last = Timer::elapsed();
    while (m_running) {
        f32 now = Timer::elapsed();
        Timestep ts = now - last; last = now;
        acc += ts;

        while (acc >= fixed_dt) { 
            m_stack.run_physics(fixed_dt); 
            acc -= fixed_dt; 
        }

        Hooks::begin_frame(m_ctx);

        m_stack.run_update(ts);
        if constexpr (AppCfg::ui) 
            m_stack.run_ui();
        Hooks::end_frame(m_ctx);

        if constexpr (AppCfg::events)
            Hooks::pump_platform(m_ctx);
    }
}

template<class AppCfg, class Ctx, class Hooks>
void Application<AppCfg, Ctx, Hooks>::close() { m_running = false; }

// template<class AppCfg, class Ctx, class Hooks>
// void Application<AppCfg, Ctx, Hooks>::begin_frame() {}

// template<class AppCfg, class Ctx, class Hooks>
// void Application<AppCfg, Ctx, Hooks>::end_frame() {}

// template<class AppCfg, class Ctx, class Hooks>
// void Application<AppCfg, Ctx, Hooks>::pump_platform() {
//     if constexpr (AppCfg::events) {
//         // Translate OS events to eng::Event then:
//         // Event e; m_stack.run_event(e);
//     }
// }

template<class AppCfg, class Ctx, class Hooks>
Ctx& Application<AppCfg, Ctx, Hooks>::context() { return m_ctx; }

template<class AppCfg, class Ctx, class Hooks>
LayerStack<Ctx>& Application<AppCfg, Ctx, Hooks>::layers() { return m_stack; }

template<class AppCfg, class Ctx, class Hooks>
CommandLineArgs Application<AppCfg, Ctx, Hooks>::args() const { return m_args; }

} // namespace eng
