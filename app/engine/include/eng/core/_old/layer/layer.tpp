#pragma once

#include "eng/core/layer/layer.h"

namespace eng {

template<class L, class AppCfg>
constexpr VTable<AppCfg> make_vtable() {
    static_assert(!(HasUI<L>    && !AppCfg::ui),     "UI callback not allowed in headless app");
    static_assert(!(HasEvent<L> && !AppCfg::events), "Event callback not allowed in headless app");

    VTable<AppCfg> v{};
    if constexpr (HasAttach<L>)  v.attach  = [](void* p){ reinterpret_cast<L*>(p)->on_attach(); };
    if constexpr (HasDetach<L>)  v.detach  = [](void* p){ reinterpret_cast<L*>(p)->on_detach(); };
    if constexpr (HasUpdate<L>)  v.update  = [](void* p, Timestep ts){ reinterpret_cast<L*>(p)->on_update(ts); };
    if constexpr (HasPhysics<L>) v.physics = [](void* p, Timestep ts){ reinterpret_cast<L*>(p)->on_physics_update(ts); };
    if constexpr (HasEvent<L>)   v.event   = [](void* p, Event& e){ reinterpret_cast<L*>(p)->on_event(e); };
    if constexpr (HasUI<L>)      v.ui      = [](void* p){ reinterpret_cast<L*>(p)->on_ui_render(); };
    return v;
}

}
