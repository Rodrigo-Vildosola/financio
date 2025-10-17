#pragma once

#include "eng/core/base.h"
#include "eng/renderer/render_pass.h"
#include "eng/renderer/renderer.h"

namespace eng {

class Shader;
class Pipeline;
// class Application;

class RendererAPI {
public:
    static void init(GraphicsContext* context);
    static void shutdown();

    static void begin_frame();
    static void end_frame();

    static void clear_color(f32 r, f32 g, f32 b, f32 a);

    static ref<Shader> create_shader(const std::string& path, const std::string& label = "");

    static ref<Pipeline> create_pipeline(const PipelineSpecification& spec);

    static ref<RenderPass> create_render_pass(const RenderPassDesc& desc);

    static GraphicsContext& get_context();
    // static wgpu::RenderPassEncoder get_current_pass_encoder();

    static const RendererStats& get_stats();
    static RendererStats& get_stats_mutable();

    // friend class Application; 

    static scope<Renderer> g_renderer;
};

} 
