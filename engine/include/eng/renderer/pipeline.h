#pragma once

#include "engpch.h"
#include "eng/renderer/pipeline_specification.h"
#include "eng/renderer/shader.h"

namespace eng {

class GraphicsContext;

class Pipeline {
public:
    Pipeline(GraphicsContext& context, const PipelineSpecification& spec);
    ~Pipeline();

    void bind(wgpu::RenderPassEncoder encoder) const;

    wgpu::BindGroupLayout get_bind_group_layout(u32 index = 0) const {
        ENG_CORE_ASSERT(index < m_bind_group_layouts.size(), "Invalid bind group layout index");
        return m_bind_group_layouts[index];
    }

    PipelineSpecification get_specification() { return m_spec; }

private:
    void create_pipeline(const PipelineSpecification& spec);

    PipelineSpecification m_spec;

    GraphicsContext& m_context;

    wgpu::RenderPipeline m_pipeline;
    wgpu::PipelineLayout m_layout;
    std::vector<wgpu::BindGroupLayout> m_bind_group_layouts;

};

    
} // namespace eng
