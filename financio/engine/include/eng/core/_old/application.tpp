#pragma once

#include "eng/core/application.h"

namespace eng {

template<class AppCfg, class Hooks>
Application<AppCfg, Hooks>::Application(CommandLineArgs args) : m_args(args) {
    Hooks::init();
}

template<class AppCfg, class Hooks>
Application<AppCfg, Hooks>::~Application() {
    Hooks::shutdown();
}

template<class AppCfg, class Hooks>
template<class L, class... Args>
L& Application<AppCfg, Hooks>::push_root_layer(Args&&... a) {
    return m_stack.template push_layer<L>(std::forward<Args>(a)...);
}

template<class AppCfg, class Hooks>
template<class L, class... Args>
L& Application<AppCfg, Hooks>::push_overlay(Args&&... a) {
    return m_stack.template push_overlay<L>(std::forward<Args>(a)...);
}

template<class AppCfg, class Hooks>
void Application<AppCfg, Hooks>::run() {
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

        Hooks::begin_frame();

        m_stack.run_update(ts);
        if constexpr (AppCfg::ui) 
            m_stack.run_ui();
        Hooks::end_frame();

        if constexpr (AppCfg::events)
            Hooks::pump_platform();
    }
}

template<class AppCfg, class Hooks>
void Application<AppCfg, Hooks>::close() { m_running = false; }

template<class AppCfg, class Hooks>
LayerStack<AppCfg>& Application<AppCfg, Hooks>::layers() { return m_stack; }

template<class AppCfg, class Hooks>
CommandLineArgs Application<AppCfg, Hooks>::args() const { return m_args; }

} // namespace eng
