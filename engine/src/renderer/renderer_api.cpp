#include "engpch.h"
#include "eng/renderer/render_pass.h"
#include "eng/renderer/renderer_api.h"
#include "eng/renderer/renderer.h"
#include "eng/renderer/shader.h"
#include "eng/renderer/material.h"
#include "eng/renderer/material_instance.h"

namespace eng {

scope<Renderer> RendererAPI::g_renderer = nullptr;

void RendererAPI::init(Context* context) {
    g_renderer = create_scope<Renderer>(*context);
    g_renderer->init();
}

void RendererAPI::shutdown() {
    g_renderer.reset();
}

void RendererAPI::begin_frame() {
    g_renderer->begin_frame();
}

void RendererAPI::end_frame() {
    g_renderer->end_frame();
}

void RendererAPI::clear_color(f32 r, f32 g, f32 b, f32 a) {
    g_renderer->clear_color(r, g, b, a);
}

ref<Shader> RendererAPI::create_shader(const std::string& path, const std::string& label) {
    return create_ref<Shader>(Shader::from_file(g_renderer->get_context(), path, label));
}

Context& RendererAPI::get_context() {
    return g_renderer->get_context();
}

// wgpu::RenderPassEncoder RendererAPI::get_current_pass_encoder() {
//     return g_renderer->get_current_pass_encoder();
// }

const RendererStats& RendererAPI::get_stats() {
    return g_renderer->get_stats();
}

RendererStats& RendererAPI::get_stats_mutable() {
    return g_renderer->get_stats_mutable();
}

ref<RenderPass> RendererAPI::create_render_pass(const RenderPassDesc& spec) {
    return g_renderer->create_render_pass(spec);
}

ref<Pipeline> RendererAPI::create_pipeline(const PipelineSpecification& spec) {
    return g_renderer->create_pipeline(spec);
}

} 
