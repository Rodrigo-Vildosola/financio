#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <cstring>
#include <algorithm>
#include <type_traits>

#include "eng/core/caps.h"
#include "eng/core/context.h"

namespace eng {

template<class Ctx>
struct VTable {
    void (*attach)(void*, Ctx&)              = nullptr;
    void (*detach)(void*, Ctx&)              = nullptr;
    void (*update)(void*, Ctx&, Timestep)    = nullptr;
    void (*physics)(void*, Ctx&, Timestep)   = nullptr;
    void (*event)(void*, Ctx&, Event&)       = nullptr;
    void (*ui)(void*, Ctx&)                  = nullptr;
};

template<class L, class AppCfg, class Ctx>
constexpr VTable<Ctx> make_vtable();

template<class Ctx>
struct AnyLayer {
    static constexpr u64 SBO = 64;
    alignas(std::max_align_t) unsigned char m_buf[SBO];
    VTable<Ctx> m_vt{};
    bool m_alive{false};
};

}


#include "eng/core/layer/layer.tpp"
