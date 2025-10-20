// eng/renderer/webgpu_texture_manager.h
#pragma once
#include "eng/ui/texture_manager.h"
#include <unordered_map>

namespace eng {

class WebGPUTextureManager : public ITextureManager {
public:
    WebGPUTextureManager(wgpu::Device device, wgpu::Queue queue);

    ImTextureID create_texture(const unsigned char* pixels, int width, int height) override;

    void destroy_texture(ImTextureID id) override;

    bool get_dimensions(ImTextureID id, int& w, int& h) override;

private:
    wgpu::Device m_device;
    wgpu::Queue m_queue;
    std::unordered_map<ImTextureID, std::pair<int,int>> m_dimensions;

};

}
