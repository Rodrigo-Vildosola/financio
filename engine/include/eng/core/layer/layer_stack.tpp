#pragma once

#include "eng/core/layer/layer_stack.h"

namespace eng {

template<class Ctx>
LayerStack<Ctx>::LayerStack(Ctx* c) : m_ctx(c) {}

template<class Ctx>
AnyLayer<Ctx>& LayerStack<Ctx>::insert_at(u64 pos) {
    AnyLayer<Ctx> empty{};
    m_layers.insert(m_layers.begin() + pos, empty);

    auto bump = [pos](std::vector<u32>& v) { 
        for (auto& x: v) 
            if (x >= pos) ++x; 
    };

    bump(m_attach); 
    bump(m_detach); 
    bump(m_update); 
    bump(m_physics); 
    bump(m_event); 
    bump(m_ui);
    return m_layers[pos];
}

template<class Ctx>
void LayerStack<Ctx>::add_indices(u32 idx) {
    auto& L = m_layers[idx];
    if (L.vt.attach)  m_attach.push_back(idx);
    if (L.vt.detach)  m_detach.push_back(idx);
    if (L.vt.update)  m_update.push_back(idx);
    if (L.vt.physics) m_physics.push_back(idx);
    if (L.vt.event)   m_event.push_back(idx);
    if (L.vt.ui)      m_ui.push_back(idx);
}

template<class Ctx>
void LayerStack<Ctx>::remove_indices(u32 idx) {
    auto rem = [idx](auto& v) {
        v.erase(std::remove(v.begin(), v.end(), idx), v.end());
        for (auto& x: v) 
            if (x > idx) --x;
    };

    rem(m_attach); 
    rem(m_detach); 
    rem(m_update); 
    rem(m_physics); 
    rem(m_event); 
    rem(m_ui);
}

template<class Ctx>
void LayerStack<Ctx>::detach_destroy(u32 idx) {
    auto& L = m_layers[idx];
    if (L.alive && L.vt.detach) { 
        L.vt.detach(L.buf, *m_ctx);
    }
    L.alive = false; // trivial types only
}

template<class Ctx>
void LayerStack<Ctx>::tombstone(u32 idx) { 
    std::memset(&m_layers[idx], 0, sizeof(AnyLayer<Ctx>)); 
}

template<class Ctx>
template<class AppCfg, class L, class... Args>
L& LayerStack<Ctx>::push_layer(Args&&... args) {
    static_assert(SBOFit<L,Ctx>, "Layer must fit SBO and be trivially copyable/destructible");

    auto& slot = insert_at(m_layer_insert);

    slot.vt = make_vtable<L,AppCfg,Ctx>();

    new (slot.buf) L(std::forward<Args>(args)...);
    slot.alive = true;

    add_indices(static_cast<u32>(m_layer_insert));

    if (slot.vt.attach) slot.vt.attach(slot.buf, *m_ctx);
    ++m_layer_insert;
    return *reinterpret_cast<L*>(slot.buf);
}

template<class Ctx>
template<class AppCfg, class L, class... Args>
L& LayerStack<Ctx>::push_overlay(Args&&... args) {
    static_assert(SBOFit<L,Ctx>, "Layer must fit SBO and be trivially copyable/destructible");

    const u32 idx = static_cast<u32>(m_layers.size());

    m_layers.emplace_back();
    auto& slot = m_layers.back();

    slot.vt = make_vtable<L,AppCfg,Ctx>();

    new (slot.buf) L(std::forward<Args>(args)...);
    slot.alive = true;

    add_indices(idx);

    if (slot.vt.attach) slot.vt.attach(slot.buf, *m_ctx);
    return *reinterpret_cast<L*>(slot.buf);
}


template<class Ctx>
void LayerStack<Ctx>::pop_layer() {
    if (m_layer_insert == 0) return;
    const u32 idx = static_cast<u32>(m_layer_insert - 1);

    detach_destroy(idx);
    remove_indices(idx);
    tombstone(idx);

    --m_layer_insert;
}

template<class Ctx>
void LayerStack<Ctx>::pop_overlay() {
    if (m_layers.size() == m_layer_insert) return;
    const u32 idx = static_cast<u32>(m_layers.size() - 1);

    detach_destroy(idx);
    remove_indices(idx);

    m_layers.pop_back();
}

template<class Ctx>
void LayerStack<Ctx>::run_update(Timestep ts) { 
    for (auto i: m_update)  
        m_layers[i].vt.update(m_layers[i].buf, *m_ctx, ts); 
}

template<class Ctx>
void LayerStack<Ctx>::run_physics(Timestep ts) { 
    for (auto i: m_physics) 
        m_layers[i].vt.physics(m_layers[i].buf, *m_ctx, ts); 
}

template<class Ctx>
void LayerStack<Ctx>::run_event(Event& e) { 
    for (auto it = m_event.rbegin(); it != m_event.rend(); ++it) 
        m_layers[*it].vt.event(m_layers[*it].buf, *m_ctx, e); 
}

template<class Ctx>
void LayerStack<Ctx>::run_ui() { 
    for (auto i: m_ui) 
        m_layers[i].vt.ui(m_layers[i].buf, *m_ctx); 
}

template<class Ctx>
Ctx& LayerStack<Ctx>::context() { 
    return *m_ctx; 
}

template<class Ctx>
const Ctx& LayerStack<Ctx>::context() const { 
    return *m_ctx; 
}


}
