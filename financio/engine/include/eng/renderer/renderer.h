#pragma once

#include "eng/core/base.h"
#include "eng/core/timestep.h"
#include "eng/renderer/buffer.h"
#include "eng/renderer/pipeline.h"
#include "eng/renderer/render_pass.h"

namespace eng {

class GraphicsContext;
class CommandQueue;

struct RendererStats {
    u32 draw_calls = 0;
    u32 mesh_count = 0;
    u32 vertex_count = 0;
    u32 index_count = 0;

    f32 frame_time_ms = 0.0f;
    f32 fps = 0.0f;

    void reset() {
        draw_calls = 0;
        mesh_count = 0;
        vertex_count = 0;
        index_count = 0;
    }
};

class Renderer {
public:
    explicit Renderer(GraphicsContext& context);
    ~Renderer();

    void init();

    // call once per frame
    void begin_frame();
    void end_frame();
    
    void begin_scene();
    void end_scene();

    void invalidate_surface_view();


    // high‐level draws
    void clear_color(f32 r, f32 g, f32 b, f32 a);

    void on_resize(u32 width, u32 height);

    ref<Pipeline> create_pipeline(const PipelineSpecification& spec);
    ref<RenderPass> create_render_pass(const RenderPassDesc& spec);

    
    const RendererStats& get_stats() const { return m_stats; }
    RendererStats& get_stats_mutable() { return m_stats; }

    GraphicsContext& get_context() { return m_context; }


private:
    // struct SceneData {    };
    // scope<SceneData> m_scene_data;

    GraphicsContext&   m_context;
    CommandQueue&    m_queue;
    
    std::vector<ref<RenderPass>> m_owned_passes;

    RendererStats m_stats;

    wgpu::Color m_clear_color;

    wgpu::TextureView    m_target_texture_view{};
    wgpu::TextureView    m_depth_texture_view{};
    wgpu::TextureFormat  m_depth_texture_format = wgpu::TextureFormat::Depth24Plus;
};

}
