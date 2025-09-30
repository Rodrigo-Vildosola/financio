#pragma once

#include "eng/core/logger.h"
#include "eng/core/macros.h"
#include <filesystem>

#ifdef FNC_ENABLE_ASSERTS

	// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
	// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
	#define FNC_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { TR##type##ERROR(msg, __VA_ARGS__); __builtin_trap(); } }
	#define FNC_INTERNAL_ASSERT_WITH_MSG(type, check, ...) FNC_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define FNC_INTERNAL_ASSERT_NO_MSG(type, check) FNC_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", FNC_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

	#define FNC_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define FNC_INTERNAL_ASSERT_GET_MACRO(...) FNC_EXPAND_MACRO( FNC_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, FNC_INTERNAL_ASSERT_WITH_MSG, FNC_INTERNAL_ASSERT_NO_MSG) )

	// Currently accepts at least the condition and one additional parameter (the message) being optional
	#define FNC_ASSERT(...) FNC_EXPAND_MACRO( FNC_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define FNC_CORE_ASSERT(...) FNC_EXPAND_MACRO( FNC_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
	#define FNC_ASSERT(...)
	#define FNC_CORE_ASSERT(...)
#endif
