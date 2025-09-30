#pragma once

#include "engpch.h"

namespace eng {

enum class ShaderStage {
    None     = 0,
    Vertex   = 1 << 0,
    Fragment = 1 << 1,
    All      = Vertex | Fragment
};

enum class VertexFormat {
    Float32x2,
    Float32x3,
    Float32x4,
    Uint32,
    // Add more as needed
};

enum class VertexStepMode {
    Vertex,
    Instance
};

inline wgpu::VertexFormat to_wgpu(VertexFormat fmt) {
    switch (fmt) {
        case VertexFormat::Float32x2: return wgpu::VertexFormat::Float32x2;
        case VertexFormat::Float32x3: return wgpu::VertexFormat::Float32x3;
        case VertexFormat::Float32x4: return wgpu::VertexFormat::Float32x4;
        case VertexFormat::Uint32:    return wgpu::VertexFormat::Uint32;
        default:                      return wgpu::VertexFormat::Uint32;
    }
}

inline wgpu::VertexStepMode to_wgpu(VertexStepMode step) {
    return step == VertexStepMode::Vertex
        ? wgpu::VertexStepMode::Vertex
        : wgpu::VertexStepMode::Instance;
}

inline wgpu::ShaderStage to_wgpu(ShaderStage stage) {
    wgpu::ShaderStage flags;
    if ((u32)stage & (u32)ShaderStage::Vertex)   flags |= wgpu::ShaderStage::Vertex;
    if ((u32)stage & (u32)ShaderStage::Fragment) flags |= wgpu::ShaderStage::Fragment;
    return flags;
}

} // namespace eng
