#pragma once

#include "eng/core/layer/layer.h"

namespace eng {

template<class L, class AppCfg, class Ctx>
constexpr VTable<Ctx> make_vtable() {
    static_assert(!(HasUI<L,Ctx>    && !AppCfg::ui),     "UI callback not allowed in headless app");
    static_assert(!(HasEvent<L,Ctx> && !AppCfg::events), "Event callback not allowed in headless app");

    VTable<Ctx> v{};
    if constexpr (HasAttach<L,Ctx>)  v.attach  = [](void* p, Ctx& c){ reinterpret_cast<L*>(p)->on_attach(c); };
    if constexpr (HasDetach<L,Ctx>)  v.detach  = [](void* p, Ctx& c){ reinterpret_cast<L*>(p)->on_detach(c); };
    if constexpr (HasUpdate<L,Ctx>)  v.update  = [](void* p, Ctx& c, Timestep ts){ reinterpret_cast<L*>(p)->on_update(c, ts); };
    if constexpr (HasPhysics<L,Ctx>) v.physics = [](void* p, Ctx& c, Timestep ts){ reinterpret_cast<L*>(p)->on_physics_update(c, ts); };
    if constexpr (HasEvent<L,Ctx>)   v.event   = [](void* p, Ctx& c, Event& e){ reinterpret_cast<L*>(p)->on_event(c, e); };
    if constexpr (HasUI<L,Ctx>)      v.ui      = [](void* p, Ctx& c){ reinterpret_cast<L*>(p)->on_ui_render(c); };
    return v;
}

}
