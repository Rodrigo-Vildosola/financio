#pragma once

#include "eng/core/layer/layer.h"


namespace eng {

template<class L, class AppCfg>
concept SBOFit = (sizeof(L) <= AnyLayer<AppCfg>::SBO) && std::is_trivially_copyable_v<L> && std::is_trivially_destructible_v<L>;

template<class AppCfg>
class LayerStack {
public:
    LayerStack() = default;

    template<class L, class... Args>
    L& push_layer(Args&&... args);

    template<class L, class... Args>
    L& push_overlay(Args&&... args);

    void pop_layer();
    void pop_overlay();

    void run_update(Timestep ts);
    void run_physics(Timestep ts);
    void run_event(Event& e);
    void run_ui();

private:
    AnyLayer<AppCfg>& insert_at(u64 pos);
    void add_indices(u32 idx);
    void remove_indices(u32 idx);
    void detach_destroy(u32 idx);
    void tombstone(u32 idx);

    std::vector<AnyLayer<AppCfg>> m_layers;
    std::vector<u32> m_attach, m_detach, m_update, m_physics, m_event, m_ui;
    u64 m_layer_insert{0};
};


}

#include "eng/core/layer/layer_stack.tpp"
