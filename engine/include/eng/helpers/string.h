#pragma once

#include "engpch.h"

namespace eng {

inline wgpu::StringView to_wgpu_string_view(std::string_view view) {
    wgpu::StringView result;
    result.length = view.length();
    result.data   = view.data(); // Safe: string_view is non-owning
    return result;
}

constexpr wgpu::StringView operator"" _wgpu(const char* str, size_t len) {
    return wgpu::StringView{ str, len };
}

}
