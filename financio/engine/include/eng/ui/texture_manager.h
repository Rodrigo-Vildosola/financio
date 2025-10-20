// eng/renderer/texture_manager.h
#pragma once
#include <webgpu/webgpu_cpp.h>
#include "eng/core/base.h"

#include "imgui.h"

namespace eng {

class ITextureManager {
public:
    virtual ~ITextureManager() = default;
    virtual ImTextureID create_texture(const unsigned char* pixels, int width, int height) = 0;
    virtual void destroy_texture(ImTextureID id) = 0;
    virtual bool get_dimensions(ImTextureID id, int& width, int& height) = 0;
};

}
