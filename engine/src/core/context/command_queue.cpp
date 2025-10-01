#include "eng/core/context/command_queue.h"
#include "eng/core/context/context_utils.h"
#include "eng/debug/profiler.h"
#include "eng/renderer/render_pass.h"
#include "eng/helpers/string.h"

namespace eng {

CommandQueue::CommandQueue(const CommandQueueProps& props) {}

CommandQueue::~CommandQueue() {}

void on_queue_work_done(wgpu::QueueWorkDoneStatus status) {
    ENG_CORE_INFO("Queued work finished with status: {}", status);
}


void CommandQueue::init(const wgpu::Device device) {
    m_device = device;
    m_queue = device.GetQueue();

    m_queue.OnSubmittedWorkDone(wgpu::CallbackMode::AllowSpontaneous, on_queue_work_done);
}


void CommandQueue::begin_frame(std::string_view label) {
    PROFILE_FUNCTION();

    ENG_CORE_ASSERT(!m_frame_active, "Command encoder already active!");

    wgpu::CommandEncoderDescriptor desc = {};
    desc.label = label;
    m_encoder = m_device.CreateCommandEncoder(&desc);

    m_frame_active = true;
}

// command_queue.cpp (inside CommandQueue)
wgpu::RenderPassEncoder CommandQueue::create_render_pass(const RenderPassDesc& desc) {
    ENG_CORE_ASSERT(m_frame_active, "Cannot create render pass without an active encoder");

    // --- Convert color attachments ---
    std::vector<wgpu::RenderPassColorAttachment> color_attachments(desc.color_attachments.size());

    for (u64 i = 0; i < desc.color_attachments.size(); ++i) {
        const auto& src = desc.color_attachments[i];

        wgpu::RenderPassColorAttachment dst = {};
        dst.view = src.view;
        dst.loadOp = src.load_op;
        dst.storeOp = src.store_op;
        dst.clearValue = src.clear_color;

        color_attachments[i] = dst;
    }

    // --- Render pass descriptor ---
    wgpu::RenderPassDescriptor render_pass_desc = {};
    render_pass_desc.label = desc.name;
    render_pass_desc.colorAttachmentCount = (u32) color_attachments.size();
    render_pass_desc.colorAttachments = color_attachments.data();

    // --- Optional depth-stencil attachment ---
    if (desc.depth_stencil_attachment.view) {
        wgpu::RenderPassDepthStencilAttachment depth_attachment = {};

        const auto& src = desc.depth_stencil_attachment;
    
        depth_attachment.view = src.view;
        depth_attachment.depthLoadOp = src.load_op;
        depth_attachment.depthStoreOp = src.store_op;
        depth_attachment.depthClearValue = src.clear_depth;
        depth_attachment.depthReadOnly = src.read_only_depth;

        render_pass_desc.depthStencilAttachment = &depth_attachment;
    }

    // --- Begin render pass ---
    m_render_pass_encoder = m_encoder.BeginRenderPass(&render_pass_desc);

    return m_render_pass_encoder;
}


void CommandQueue::end_render_pass() {
    if (m_render_pass_encoder) {
        // wgpuRenderPassEncoderEnd(m_render_pass_encoder);
        m_render_pass_encoder.End();
        // wgpuRenderPassEncoderRelease(m_render_pass_encoder);
        m_render_pass_encoder = nullptr;
    }
}




void CommandQueue::end_frame() {
    ENG_CORE_ASSERT(m_frame_active, "No active command encoder!");

    wgpu::CommandBufferDescriptor desc = {};
    desc.label = "Command Buffer";
    wgpu::CommandBuffer cmd_buffer = m_encoder.Finish(&desc);

    m_encoder = nullptr;

    m_queue.Submit(1, &cmd_buffer);

    m_frame_active = false;
}

void CommandQueue::submit_now() {
    if (m_frame_active) {
        end_frame();
    }
}


void CommandQueue::add_marker(std::string_view label) {
    ENG_CORE_ASSERT(m_frame_active, "Can't add marker: no active encoder.");
    m_encoder.InsertDebugMarker(label);
}


void CommandQueue::poll([[maybe_unused]] bool yield_to_browser) {
    PROFILE_FUNCTION();

    wgpu_poll_events(m_device, false);
}


scope<CommandQueue> CommandQueue::create(const CommandQueueProps& props) {
    return create_scope<CommandQueue>(props);
}


}

