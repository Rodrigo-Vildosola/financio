#pragma once

#include "engpch.h"
#include "eng/renderer/context/context.h"


namespace eng {

struct UniformBuffer {
    wgpu::Buffer buffer = nullptr;
    u64 size = 0;
    u32 binding = 0;
    const char* label = "Uniform Buffer";
};

struct VertexBuffer {
    wgpu::Buffer buffer = nullptr;
    u64 size = 0;
    u32 slot = 0; // Index to bind the vertex buffer (setVertexBuffer)
    const char* label = "Vertex Buffer";
};

struct IndexBuffer {
    wgpu::Buffer buffer = nullptr;
    u64 size = 0;
    wgpu::IndexFormat format = wgpu::IndexFormat::Uint32;
    const char* label = "Index Buffer";
};

struct StorageBuffer {
    wgpu::Buffer buffer = nullptr;
    u64 size = 0;
    u32 binding = 0;
    const char* label = "Storage Buffer";
};


class Buffer {
public:
    static void example(wgpu::Instance instance, wgpu::Device device, wgpu::Queue queue);


    static wgpu::Buffer create(
        wgpu::Device device,
        wgpu::Queue queue,
        const void* data,
        size_t size,
        wgpu::BufferUsage usage,
        const char* label = "Unnamed Buffer"
    );

    static UniformBuffer create_uniform_buffer(
        GraphicsContext& ctx,
        const void* data,
        u64 size,
        u32 binding,
        const char* label = "Uniform Buffer"
    );

    static VertexBuffer create_vertex_buffer(
        GraphicsContext& ctx,
        const void* data,
        u64 size,
        u32 slot = 0,
        const char* label = "Vertex Buffer"
    );

    static IndexBuffer create_index_buffer(
        GraphicsContext& ctx,
        const void* data,
        u64 size,
        wgpu::IndexFormat format = wgpu::IndexFormat::Uint32,
        const char* label = "Index Buffer"
    );

    static StorageBuffer create_storage_buffer(
        GraphicsContext& ctx,
        const void* data,
        u64 size,
        u32 binding,
        const char* label = "Storage Buffer"
    );
};

void fetch_buffer_data_sync(wgpu::Instance instance, wgpu::Buffer buffer, std::function<void(const void*)> process_buffer_data);

}
