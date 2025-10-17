#pragma once

#include "eng/core/base.h"
#include <webgpu/webgpu_cpp.h>

namespace eng {

class RenderPassDesc;

class CommandQueueProps {

};

class CommandQueue {
public:

    explicit CommandQueue(const CommandQueueProps& props = CommandQueueProps());
    ~CommandQueue();

    void init(const wgpu::Device device);
    void begin_frame(std::string_view label = "Frame Command Encoder");
    void end_frame();

    void add_marker(std::string_view label);
    void submit_now();
    wgpu::RenderPassEncoder create_render_pass(const RenderPassDesc& desc);

    void end_render_pass();
    void poll([[maybe_unused]] bool yield_to_browser); // Poll/tick device for async processing

    wgpu::Queue get_native_queue() const { return m_queue; }

    wgpu::RenderPassEncoder get_render_pass_encoder() const { return m_render_pass_encoder; }

    static scope<CommandQueue> create(const CommandQueueProps& props = CommandQueueProps());


private:
    wgpu::Device m_device = nullptr;
    wgpu::Queue m_queue = nullptr;
    wgpu::CommandEncoder m_encoder = nullptr;
    wgpu::RenderPassEncoder m_render_pass_encoder = nullptr;


    bool m_frame_active = false;
};
    
} // namespace eng
