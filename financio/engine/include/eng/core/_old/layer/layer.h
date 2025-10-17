#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <cstring>
#include <algorithm>
#include <type_traits>

#include "eng/core/caps.h"
#include "eng/core/timestep.h"
#include "eng/events/event.h"

namespace eng {

template<class AppCfg>
struct VTable {
    void (*attach)(void*)                    = nullptr;
    void (*detach)(void*)                    = nullptr;
    void (*update)(void*, Timestep)          = nullptr;
    void (*physics)(void*, Timestep)         = nullptr;
    void (*event)(void*, Event&)             = nullptr;
    void (*ui)(void*)                        = nullptr;
};

template<class L, class AppCfg>
constexpr VTable<AppCfg> make_vtable();

template<class AppCfg>
struct AnyLayer {
    static constexpr u64 SBO = 64;
    alignas(std::max_align_t) unsigned char m_buf[SBO];
    VTable<AppCfg> m_vt{};
    bool m_alive{false};
};

}


#include "eng/core/layer/layer.tpp"
