// eng/renderer/webgpu_texture_manager.h
#pragma once
#include "eng/ui/webgpu_texture_manager.h"
#include <unordered_map>

namespace eng {

WebGPUTextureManager::WebGPUTextureManager(wgpu::Device device, wgpu::Queue queue) : m_device(device), m_queue(queue) {}


ImTextureID WebGPUTextureManager::create_texture(const unsigned char* pixels, int width, int height) {
    wgpu::TextureDescriptor td{};
    td.size = { (uint32_t)width, (uint32_t)height, 1 };
    td.format = wgpu::TextureFormat::RGBA8Unorm;
    td.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst;
    wgpu::Texture tex = m_device.CreateTexture(&td);

    wgpu::TexelCopyTextureInfo dst{};
    dst.texture = tex;
    dst.mipLevel = 0;
    dst.origin = { 0, 0, 0 };
    dst.aspect = wgpu::TextureAspect::All;

    wgpu::TexelCopyBufferLayout layout{};
    layout.offset = 0;
    layout.bytesPerRow = width * 4;
    layout.rowsPerImage = height;

    wgpu::Extent3D extent{ (uint32_t)width, (uint32_t)height, 1 };
    m_queue.WriteTexture(&dst, pixels, width * height * 4, &layout, &extent);

    wgpu::TextureView view = tex.CreateView();
    m_dimensions[(ImTextureID) view.Get()] = { width, height };

    return reinterpret_cast<ImTextureID>(view.Get());
}

void WebGPUTextureManager::destroy_texture(ImTextureID id) {
    if (!id) return;
    auto view = reinterpret_cast<WGPUTextureView>(id);
    m_dimensions.erase(id);
    wgpuTextureViewRelease(view);
}

bool WebGPUTextureManager::get_dimensions(ImTextureID id, int& w, int& h) {
    auto it = m_dimensions.find(id);
    if (it == m_dimensions.end()) return false;
    w = it->second.first;
    h = it->second.second;
    return true;
}

}
