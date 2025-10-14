#include "eng/renderer/render_pass.h"
#include "eng/renderer/buffer.h"
#include "eng/renderer/context/command_queue.h"

#include "eng/core/assert.h"
#include "eng/core/debug/profiler.h"
#include "eng/renderer/renderer_api.h"

namespace eng {

RenderPass::RenderPass(const RenderPassDesc& desc, CommandQueue& queue)
    : m_desc(desc), m_queue(queue) {}

// RenderPass::~RenderPass() {}

void RenderPass::begin() {
    PROFILE_FUNCTION();
    m_queue.begin_frame(m_desc.name);
    m_queue.add_marker("Begin " + std::string(m_desc.name));
    m_pass_encoder = m_queue.create_render_pass(m_desc);
}



void RenderPass::end() {
    PROFILE_FUNCTION();

    ENG_CORE_ASSERT(m_pass_encoder, "Render pass encoder is null at end!");

    // for (DrawBatch& b : m_batches) {
    //     if (b.instance_count == 0) {
    //         ENG_CORE_WARN("Skipping batch with zero instances");
    //         continue;
    //     }

    //     const u64 needed = b.instance_data.size();
    //     ENG_CORE_ASSERT(needed > 0, "Instance data size must be > 0");

    //     // Buffer upload or reallocation
    //     if (!b.instance_buffer || needed > b.buffer_capacity) {
    //         StorageBuffer created = Buffer::create_storage_buffer(
    //             RendererAPI::get_context(),
    //             b.instance_data.data(),
    //             needed,
    //             b.binding,
    //             "Instance Storage"
    //         );

    //         ENG_CORE_ASSERT(created.buffer.Get(), "Failed to create instance buffer!");
    //         b.instance_buffer = created.buffer;
    //         b.buffer_capacity = needed;
    //     } else {
    //         ENG_CORE_ASSERT(b.instance_buffer.Get(), "Instance buffer is null before WriteBuffer!");
    //         m_queue.get_native_queue().WriteBuffer(
    //             b.instance_buffer,
    //             0,
    //             b.instance_data.data(),
    //             needed
    //         );
    //     }

    //     // Material & pipeline binding
    //     ENG_CORE_ASSERT(b.material, "Material is null in draw batch!");
    //     b.material->bind_storage_buffer(
    //         b.group, 
    //         b.binding, 
    //         b.instance_buffer
    //     );
    //     b.material->bind(m_pass_encoder);

    //     // Vertex & index buffer validation
    //     const auto& vb = b.mesh->get_vertex_buffer();
    //     const auto& ib = b.mesh->get_index_buffer();

    //     ENG_CORE_ASSERT(vb.buffer.Get(), "Vertex buffer is null");
    //     ENG_CORE_ASSERT(vb.size > 0, "Vertex buffer size is 0");

    //     ENG_CORE_ASSERT(ib.buffer.Get(), "Index buffer is null");
    //     ENG_CORE_ASSERT(ib.size > 0, "Index buffer size is 0");

    //     u32 index_count = b.mesh->get_index_count();
    //     ENG_CORE_ASSERT(index_count > 0, "Mesh has zero index count");

    //     // Bind buffers and draw
    //     m_pass_encoder.SetVertexBuffer(0, vb.buffer, 0, vb.size);
    //     m_pass_encoder.SetIndexBuffer(ib.buffer, ib.format, 0, ib.size);
    //     m_pass_encoder.DrawIndexed(index_count, b.instance_count, 0, 0, 0);

    //     ENG_CORE_TRACE("DrawIndexed | Mesh: {}, Material: {}, Indices: {}, Instances: {}",
    //         (void*)b.mesh.get(), (void*)b.material.get(), index_count, b.instance_count);
    // }

    m_batches.clear();

    m_queue.end_render_pass();
    m_queue.add_marker("End " + std::string(m_desc.name));
    m_queue.end_frame();

    m_pass_encoder = nullptr;
}


void RenderPass::add_dependency(RenderPass* pass) {
    m_dependencies.push_back(pass);
}

const std::vector<RenderPass*>& RenderPass::get_dependencies() const {
    return m_dependencies;
}

} // namespace eng 
