#pragma once

#include <cstdint>
#include <memory>

#include "eng/core/platform_detection.h"
#include "eng/core/assert.h"


#ifdef ENG_DEBUG
	#if defined(ENG_PLATFORM_WINDOWS)
		#define ENG_DEBUGBREAK() __debugbreak()
	#elif defined(ENG_PLATFORM_LINUX)
		#include <signal.h>
		#define ENG_DEBUGBREAK() raise(SIGTRAP)
	#elif defined(ENG_PLATFORM_MACOS)
        #include <signal.h>
        #define ENG_DEBUGBREAK() raise(SIGTRAP)
	#endif

	// #define ENG_ENABLE_ASSERTS
#else
	#define ENG_DEBUGBREAK()
#endif

namespace eng {

using i8  = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t; // unsigned int
using u64 = std::uint64_t; // unsigned long long

using f32 = float;
using f64 = double;

template<typename T>
using scope = std::unique_ptr<T>;

template<typename T, typename ... Args>
constexpr scope<T> create_scope(Args&& ... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using ref = std::shared_ptr<T>;

template<typename T, typename ... Args>
constexpr ref<T> create_ref(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

}
