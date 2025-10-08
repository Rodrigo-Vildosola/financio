#pragma once

#include "engpch.h"

#include <webgpu/webgpu_cpp.h>

namespace eng {

class Window;
class CommandQueue;

struct GraphicsContextProps {
    // Placeholder for future context settings like:
    // bool enableValidation = true;
    // std::string preferredAdapterName;
};

class GraphicsContext {
public:
    explicit GraphicsContext(const GraphicsContextProps& props = GraphicsContextProps());
    ~GraphicsContext();

    void init(Window* window_handle);
    void swap_buffers();

    wgpu::Device get_native_device() { return m_device; }
    wgpu::Instance get_native_instance() { return m_instance; }
    CommandQueue* get_queue() { return m_queue.get(); }

    wgpu::TextureView get_next_surface_view();

    void configure_surface(wgpu::TextureFormat preferred_format);

    wgpu::TextureFormat get_preferred_format() const { return m_surface_format; }

    std::pair<u32, u32> get_framebuffer_size();

    static scope<Context> create(const ContextProps& props = ContextProps());

private:
    Window* m_window_handle = nullptr;
    ContextProps m_props;
    
    wgpu::Instance m_instance = nullptr;
    wgpu::Device  m_device = nullptr;
    wgpu::Surface m_surface = nullptr;

    wgpu::TextureFormat m_surface_format = wgpu::TextureFormat::Undefined;

    scope<CommandQueue> m_queue;


};

} // namespace eng
