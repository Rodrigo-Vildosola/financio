#pragma once

#include "eng/core/context.h"

namespace eng {

template<class... Services>
template<class T, class... Args>
T& Context<Services...>::emplace(Args&&... args) {
    static_assert(contains_v<T>, "T is not a member of this Context");
    return (get<T>() = T(std::forward<Args>(args)...));
}

template<class... Services>
template<class T>
constexpr bool Context<Services...>::has() const {
    static_assert(contains_v<T>, "T is not a member of this Context");
    return true;
}

template<class... Services>
template<class T>
T& Context<Services...>::get() {
    static_assert(contains_v<T>, "T is not a member of this Context");
    return std::get<T>(m_store);
}

template<class... Services>
template<class T>
const T& Context<Services...>::get() const {
    static_assert(contains_v<T>, "T is not a member of this Context");
    return std::get<T>(m_store);
}

} // namespace eng
