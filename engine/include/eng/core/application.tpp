#pragma once

#include "eng/core/application.h"

namespace eng {

template<class AppCfg, class Ctx>
Application<AppCfg, Ctx>::Application(CommandLineArgs args) : m_args(args) {}

template<class AppCfg, class Ctx>
template<class L, class... Args>
L& Application<AppCfg, Ctx>::push_root_layer(Args&&... a) {
    return m_stack.template push_layer<AppCfg, L>(std::forward<Args>(a)...);
}

template<class AppCfg, class Ctx>
template<class L, class... Args>
L& Application<AppCfg, Ctx>::push_overlay(Args&&... a) {
    return m_stack.template push_overlay<AppCfg, L>(std::forward<Args>(a)...);
}

template<class AppCfg, class Ctx>
void Application<AppCfg, Ctx>::run() {

    Timer::init();

    const Timestep fixed_dt(1.0f/60.0f);
    float acc = 0.f;
    float last = Timer::elapsed();
    while (m_running) {
        float now = Timer::elapsed();
        Timestep ts = now - last; last = now;
        acc += ts;

        while (acc >= fixed_dt) { 
            m_stack.run_physics(fixed_dt); 
            acc -= fixed_dt; 
        }

        begin_frame();
        m_stack.run_update(ts);
        if constexpr (AppCfg::ui) 
            m_stack.run_ui();
        end_frame();

        pump_platform();
    }
}

template<class AppCfg, class Ctx>
void Application<AppCfg, Ctx>::close() { m_running = false; }

template<class AppCfg, class Ctx>
void Application<AppCfg, Ctx>::begin_frame() {}

template<class AppCfg, class Ctx>
void Application<AppCfg, Ctx>::end_frame() {}

template<class AppCfg, class Ctx>
void Application<AppCfg, Ctx>::pump_platform() {
    if constexpr (AppCfg::events) {
        // Translate OS events to eng::Event then:
        // Event e; m_stack.run_event(e);
    }
}

template<class AppCfg, class Ctx>
Ctx& Application<AppCfg, Ctx>::context() { return m_ctx; }

template<class AppCfg, class Ctx>
LayerStack<Ctx>& Application<AppCfg, Ctx>::layers() { return m_stack; }

template<class AppCfg, class Ctx>
CommandLineArgs Application<AppCfg, Ctx>::args() const { return m_args; }

} // namespace eng
