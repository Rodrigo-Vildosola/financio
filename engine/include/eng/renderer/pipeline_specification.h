#pragma once

#include "engpch.h"
#include "eng/renderer/shader.h"

namespace eng {

struct VertexAttributeSpec {
    u32 shader_location = 0;
    wgpu::VertexFormat format = wgpu::VertexFormat::Float32x3;
    u64 offset = 0;
};

struct VertexBufferLayoutSpec {
    u64 stride = 0;
    wgpu::VertexStepMode step_mode = wgpu::VertexStepMode::Vertex;
    std::vector<VertexAttributeSpec> attributes;
};

struct StorageBufferSpec {
    u32 binding;
    u32 group = 1;
    u64 size;
    wgpu::ShaderStage visibility;
};

struct UniformBufferSpec {
    u32 binding;
    u32 group = 0;
    u64 size;
    wgpu::ShaderStage visibility;
};

struct PipelineSpecification {
    ref<Shader> shader = nullptr;

    wgpu::TextureFormat surface_format;
    std::vector<VertexBufferLayoutSpec> vertex_buffers;

    std::vector<UniformBufferSpec> uniforms;
    std::vector<StorageBufferSpec> storages;

    wgpu::TextureFormat depth_format;
};

} // namespace eng
