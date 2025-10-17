#pragma once

#define ENG_EXPAND_MACRO(x) x
#define ENG_STRINGIFY_MACRO(x) #x


#define BIT(x) (1 << x)

#define ENG_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

