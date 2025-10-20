#pragma once

#include "eng/core/layer/layer_stack.h"

namespace eng {

template<class AppCfg>
AnyLayer<AppCfg>& LayerStack<AppCfg>::insert_at(u64 pos) {
    AnyLayer<AppCfg> empty{};
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

template<class AppCfg>
void LayerStack<AppCfg>::add_indices(u32 idx) {
    auto& L = m_layers[idx];
    if (L.m_vt.attach)  m_attach.push_back(idx);
    if (L.m_vt.detach)  m_detach.push_back(idx);
    if (L.m_vt.update)  m_update.push_back(idx);
    if (L.m_vt.physics) m_physics.push_back(idx);
    if (L.m_vt.event)   m_event.push_back(idx);
    if (L.m_vt.ui)      m_ui.push_back(idx);
}

template<class AppCfg>
void LayerStack<AppCfg>::remove_indices(u32 idx) {
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

template<class AppCfg>
void LayerStack<AppCfg>::detach_destroy(u32 idx) {
    auto& L = m_layers[idx];
    if (L.m_alive && L.m_vt.detach) { 
        L.m_vt.detach(L.m_buf);
    }
    L.m_alive = false; // trivial types only
}

template<class AppCfg>
void LayerStack<AppCfg>::tombstone(u32 idx) { 
    std::memset(&m_layers[idx], 0, sizeof(AnyLayer<AppCfg>)); 
}

template<class AppCfg>
template<class L, class... Args>
L& LayerStack<AppCfg>::push_layer(Args&&... args) {
    static_assert(SBOFit<L,AppCfg>, "Layer must fit SBO and be trivially copyable/destructible");

    auto& slot = insert_at(m_layer_insert);

    slot.m_vt = make_vtable<L,AppCfg>();

    new (slot.m_buf) L(std::forward<Args>(args)...);
    slot.m_alive = true;

    add_indices(static_cast<u32>(m_layer_insert));

    if (slot.m_vt.attach) slot.m_vt.attach(slot.m_buf);
    ++m_layer_insert;
    return *reinterpret_cast<L*>(slot.m_buf);
}

template<class AppCfg>
template<class L, class... Args>
L& LayerStack<AppCfg>::push_overlay(Args&&... args) {
    static_assert(SBOFit<L,AppCfg>, "Layer must fit SBO and be trivially copyable/destructible");

    const u32 idx = static_cast<u32>(m_layers.size());

    m_layers.emplace_back();
    auto& slot = m_layers.back();

    slot.m_vt = make_vtable<L,AppCfg>();

    new (slot.m_buf) L(std::forward<Args>(args)...);
    slot.m_alive = true;

    add_indices(idx);

    if (slot.m_vt.attach) slot.m_vt.attach(slot.m_buf);
    return *reinterpret_cast<L*>(slot.m_buf);
}


template<class AppCfg>
void LayerStack<AppCfg>::pop_layer() {
    if (m_layer_insert == 0) return;
    const u32 idx = static_cast<u32>(m_layer_insert - 1);

    detach_destroy(idx);
    remove_indices(idx);
    tombstone(idx);

    --m_layer_insert;
}

template<class AppCfg>
void LayerStack<AppCfg>::pop_overlay() {
    if (m_layers.size() == m_layer_insert) return;
    const u32 idx = static_cast<u32>(m_layers.size() - 1);

    detach_destroy(idx);
    remove_indices(idx);

    m_layers.pop_back();
}

template<class AppCfg>
void LayerStack<AppCfg>::run_update(Timestep ts) { 
    for (auto i: m_update)  
        m_layers[i].m_vt.update(m_layers[i].m_buf, ts); 
}

template<class AppCfg>
void LayerStack<AppCfg>::run_physics(Timestep ts) { 
    for (auto i: m_physics) 
        m_layers[i].m_vt.physics(m_layers[i].m_buf, ts); 
}

template<class AppCfg>
void LayerStack<AppCfg>::run_event(Event& e) { 
    for (auto it = m_event.rbegin(); it != m_event.rend(); ++it) 
        m_layers[*it].m_vt.event(m_layers[*it].m_buf, e); 
}

template<class AppCfg>
void LayerStack<AppCfg>::run_ui() { 
    for (auto i: m_ui) 
        m_layers[i].m_vt.ui(m_layers[i].m_buf); 
}


}
