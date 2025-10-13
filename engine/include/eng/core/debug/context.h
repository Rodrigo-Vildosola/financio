#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

namespace eng {

// Typed context: fixed set of systems/services at compile time.
// No hashing, no type-erasure, zero overhead in use sites.
template<class... Services>
class Context {
public:
    // Compile-time membership test
    template<class T>
    static constexpr bool contains_v = (std::is_same_v<std::remove_cv_t<std::remove_reference_t<T>>, Services> || ...);

    // Construct in-place if present in the pack
    template<class T, class... Args>
    T& emplace(Args&&... args);

    template<class T>
    constexpr bool has() const;


    template<class T>
    T& get();

    template<class T>
    const T& get() const;

private:
    std::tuple<Services...> m_store{};
};

} // namespace eng

#include "eng/core/context.tpp"
