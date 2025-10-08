#pragma once

#include "eng/core/layer/layer.h"


namespace eng {

template<class L, class Ctx>
concept SBOFit = (sizeof(L) <= AnyLayer<Ctx>::SBO) && std::is_trivially_copyable_v<L> && std::is_trivially_destructible_v<L>;

template<class Ctx>
class LayerStack {
public:
    explicit LayerStack(Ctx* c);

    template<class AppCfg, class L, class... Args>
    L& push_layer(Args&&... args);

    template<class AppCfg, class L, class... Args>
    L& push_overlay(Args&&... args);

    void pop_layer();
    void pop_overlay();

    void run_update(Timestep ts);
    void run_physics(Timestep ts);
    void run_event(Event& e);
    void run_ui();

    Ctx& context();
    const Ctx& context() const;

private:
    AnyLayer<Ctx>& insert_at(u64 pos);
    void add_indices(u32 idx);
    void remove_indices(u32 idx);
    void detach_destroy(u32 idx);
    void tombstone(u32 idx);

    std::vector<AnyLayer<Ctx>> m_layers;
    std::vector<u32> m_attach, m_detach, m_update, m_physics, m_event, m_ui;
    u64 m_layer_insert{0};
    Ctx* m_ctx;
};


}

#include "eng/core/layer/layer_stack.tpp"
