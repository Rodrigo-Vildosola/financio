#include "eng/core/assert.h"
#include "eng/core/base.h"
#include "eng/core/logger.h"
#include "eng/debug/profiler.h"
#include "eng/renderer/render_pass.h"
#include "engpch.h"

#include "eng/resources/resource_manager.h"
#include "eng/core/context/command_queue.h"
#include "eng/renderer/renderer.h"
#include "eng/renderer/material.h"
#include "eng/helpers/string.h"

namespace eng {

Renderer::Renderer(Context& ctx) : m_context(ctx), m_queue(*ctx.get_queue()) {
    m_scene_data = create_scope<SceneData>();
}

Renderer::~Renderer() {}

void Renderer::init() {
    m_depth_texture_format = wgpu::TextureFormat::Depth24Plus;

    auto [fb_width, fb_height] = m_context.get_framebuffer_size();

    on_resize(fb_width, fb_height);
}

ref<Pipeline> Renderer::create_pipeline(const PipelineSpecification& spec) {
    PROFILE_FUNCTION();
    // Fill in internal fields like surface format and depth view
    PipelineSpecification internal_spec = spec;
    internal_spec.surface_format = m_context.get_preferred_format();
    internal_spec.depth_format = m_depth_texture_format;

    return create_ref<Pipeline>(m_context, internal_spec);
}

ref<RenderPass> Renderer::create_render_pass(const RenderPassDesc& desc) {
    RenderPassDesc final_desc = desc;

    FNC_CORE_INFO("Creating RenderPass: {}", final_desc.name);

    // Debug color attachments
    for (size_t i = 0; i < final_desc.color_attachments.size(); ++i) {
        RenderPassAttachment& attachment = final_desc.color_attachments[i];

        if (!attachment.view) {
            FNC_CORE_WARN("Color attachment [{}] has no view. Injecting default target view.", i);
            FNC_CORE_ASSERT(m_target_texture_view, "Target texture view is null! Call begin_frame() before creating a render pass.");
            attachment.view = m_target_texture_view;
        } else {
            FNC_CORE_INFO("Color attachment [{}] already has a view set.", i);
        }

        FNC_CORE_INFO("    LoadOp: {}, StoreOp: {}, ClearColor: ({}, {}, {}, {})",
            attachment.load_op,
            attachment.store_op,
            attachment.clear_color.r,
            attachment.clear_color.g,
            attachment.clear_color.b,
            attachment.clear_color.a
        );
    }

    // Debug depth attachment
    if (!final_desc.depth_stencil_attachment.view) {
        FNC_CORE_WARN("Depth attachment has no view. Injecting default depth view.");
        FNC_CORE_ASSERT(m_depth_texture_view, "Depth texture view is null! Call begin_frame() before creating a render pass.");
        final_desc.depth_stencil_attachment.view = m_depth_texture_view;
    } else {
        FNC_CORE_INFO("Depth attachment already has a view set.");
    }

    FNC_CORE_INFO("    Depth Clear: {}, ReadOnly: {}",
        final_desc.depth_stencil_attachment.clear_depth,
        final_desc.depth_stencil_attachment.read_only_depth
    );

    auto pass = create_ref<RenderPass>(final_desc, m_queue);
    m_owned_passes.push_back(pass);
    return pass;
}


void Renderer::begin_frame() {
    PROFILE_FUNCTION();

    m_stats.reset();
    m_target_texture_view = m_context.get_next_surface_view();
}

void Renderer::end_frame() {
    PROFILE_FUNCTION();

    m_context.swap_buffers();
    m_queue.poll(false);
}


void Renderer::clear_color(f32 r, f32 g, f32 b, f32 a) {
    PROFILE_FUNCTION();

    m_clear_color = {
        .r = r,
        .g = g,
        .b = b,
        .a = a
    };
}

void Renderer::on_resize(u32 width, u32 height) {
    FNC_CORE_INFO("Resizing renderer to {}x{}", width, height);

    const auto& device = m_context.get_native_device();

    wgpu::TextureDescriptor depth_texture_desc = {};
    depth_texture_desc.label = "Z Buffer";
    depth_texture_desc.usage = wgpu::TextureUsage::RenderAttachment;
    depth_texture_desc.size = { width, height, 1 };
    depth_texture_desc.format = m_depth_texture_format;
    
    wgpu::Texture depth_texture = device.CreateTexture(&depth_texture_desc);

    m_depth_texture_view = depth_texture.CreateView();
}

void Renderer::invalidate_surface_view() {
    m_target_texture_view = nullptr;
}

}
