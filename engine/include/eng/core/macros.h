#pragma once

#define FNC_EXPAND_MACRO(x) x
#define FNC_STRINGIFY_MACRO(x) #x


#define BIT(x) (1 << x)

#define FNC_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

