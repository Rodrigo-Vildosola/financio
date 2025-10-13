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

template<class T> concept HasAttach   = requires(T& t){ t.on_attach(); };
template<class T> concept HasDetach   = requires(T& t){ t.on_detach(); };
template<class T> concept HasUpdate   = requires(T& t, Timestep dt){ t.on_update(dt); };
template<class T> concept HasPhysics  = requires(T& t, Timestep dt){ t.on_physics_update(dt); };
template<class T> concept HasEvent    = requires(T& t, Event& e){ t.on_event(e); };
template<class T> concept HasUI       = requires(T& t){ t.on_ui_render(); };

} // namespace eng
