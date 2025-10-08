// eng/core/caps.hpp
#pragma once
#include "eng/core/timestep.h"
#include "eng/events/event.h"

namespace eng {

struct HeadlessConfig { 
    static constexpr bool ui = false;
    static constexpr bool events = false; 
};

struct GuiConfig { 
    static constexpr bool ui=true;
    static constexpr bool events=true;
};

template<class T, class C> concept HasAttach   = requires(T& t, C& c){ t.on_attach(c); };
template<class T, class C> concept HasDetach   = requires(T& t, C& c){ t.on_detach(c); };
template<class T, class C> concept HasUpdate   = requires(T& t, C& c, Timestep dt){ t.on_update(c, dt); };
template<class T, class C> concept HasPhysics  = requires(T& t, C& c, Timestep dt){ t.on_physics_update(c, dt); };
template<class T, class C> concept HasEvent    = requires(T& t, C& c, Event& e){ t.on_event(c, e); };
template<class T, class C> concept HasUI       = requires(T& t, C& c){ t.on_ui_render(c); };

} // namespace eng
