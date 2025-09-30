#include "eng/renderer/render_pass.h"
#include "eng/renderer/buffer.h"
#include "eng/core/context/command_queue.h"

#include "eng/core/assert.h"
#include "eng/debug/profiler.h"
#include "eng/renderer/material_instance.h"
#include "eng/renderer/mesh.h"
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

void RenderPass::submit(const ref<Mesh>& mesh, const ref<MaterialInstance>& material, const void* instance, u32 size, u32 binding, u32 group) {
    if (!m_pass_encoder) return;

    for (DrawBatch& b : m_batches) {
        if (b.mesh == mesh &&
            b.material == material &&
            b.binding == binding &&
            b.group == group &&
            b.instance_stride == size) {

            const u8* src = (const u8*) instance;
            b.instance_data.insert(
                b.instance_data.end(), 
                src, 
                src + size
            );
            b.instance_count++;
            return;
        }
    }

    DrawBatch nb;
    nb.mesh = mesh;
    nb.material = material;
    nb.binding = binding;
    nb.group = group;
    nb.instance_stride = size;
    nb.instance_count = 1;

    if (instance && size > 0) {
        nb.instance_data.resize(size);
        memcpy(nb.instance_data.data(), instance, size);
    } else {
        FNC_CORE_ERROR("Invalid instance pointer or size! Pointer: {}, Size: {}", (void*)instance, size);
        return;
    }

    m_batches.push_back(std::move(nb));

}



void RenderPass::end() {
    PROFILE_FUNCTION();

    FNC_CORE_ASSERT(m_pass_encoder, "Render pass encoder is null at end!");

    for (DrawBatch& b : m_batches) {
        if (b.instance_count == 0) {
            FNC_CORE_WARN("Skipping batch with zero instances");
            continue;
        }

        const u64 needed = b.instance_data.size();
        FNC_CORE_ASSERT(needed > 0, "Instance data size must be > 0");

        // Buffer upload or reallocation
        if (!b.instance_buffer || needed > b.buffer_capacity) {
            StorageBuffer created = Buffer::create_storage_buffer(
                RendererAPI::get_context(),
                b.instance_data.data(),
                needed,
                b.binding,
                "Instance Storage"
            );

            FNC_CORE_ASSERT(created.buffer.Get(), "Failed to create instance buffer!");
            b.instance_buffer = created.buffer;
            b.buffer_capacity = needed;
        } else {
            FNC_CORE_ASSERT(b.instance_buffer.Get(), "Instance buffer is null before WriteBuffer!");
            m_queue.get_native_queue().WriteBuffer(
                b.instance_buffer,
                0,
                b.instance_data.data(),
                needed
            );
        }

        // Material & pipeline binding
        FNC_CORE_ASSERT(b.material, "Material is null in draw batch!");
        b.material->bind_storage_buffer(
            b.group, 
            b.binding, 
            b.instance_buffer
        );
        b.material->bind(m_pass_encoder);

        // Vertex & index buffer validation
        const auto& vb = b.mesh->get_vertex_buffer();
        const auto& ib = b.mesh->get_index_buffer();

        FNC_CORE_ASSERT(vb.buffer.Get(), "Vertex buffer is null");
        FNC_CORE_ASSERT(vb.size > 0, "Vertex buffer size is 0");

        FNC_CORE_ASSERT(ib.buffer.Get(), "Index buffer is null");
        FNC_CORE_ASSERT(ib.size > 0, "Index buffer size is 0");

        u32 index_count = b.mesh->get_index_count();
        FNC_CORE_ASSERT(index_count > 0, "Mesh has zero index count");

        // Bind buffers and draw
        m_pass_encoder.SetVertexBuffer(0, vb.buffer, 0, vb.size);
        m_pass_encoder.SetIndexBuffer(ib.buffer, ib.format, 0, ib.size);
        m_pass_encoder.DrawIndexed(index_count, b.instance_count, 0, 0, 0);

        FNC_CORE_TRACE("DrawIndexed | Mesh: {}, Material: {}, Indices: {}, Instances: {}",
            (void*)b.mesh.get(), (void*)b.material.get(), index_count, b.instance_count);
    }

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
