#pragma once


#include "eng/core/logger.h"
#include "engpch.h"

namespace eng {

class CommandQueue;
class Mesh;
class MaterialInstance;

struct RenderPassAttachment {
    wgpu::TextureView view = nullptr;

    wgpu::LoadOp load_op = wgpu::LoadOp::Clear;
    wgpu::StoreOp store_op = wgpu::StoreOp::Store;

    wgpu::Color clear_color = {0.0f, 0.0f, 0.0f, 1.0f}; // Used for color
    float clear_depth = 1.0f;                        // Used for depth
    uint32_t clear_stencil = 0;                      // Optional

    bool read_only_depth = false;                    // For depth-only passes
};

struct RenderPassDesc {
    std::string_view name = "UnnamedPass";

    std::vector<RenderPassAttachment> color_attachments;
    RenderPassAttachment depth_stencil_attachment;
};

using RenderPassID = u64;


class RenderPass {
public:
    RenderPass(const RenderPassDesc& desc, CommandQueue& queue);

    void begin();
    void end();

    template<typename T>
    void submit(const ref<Mesh>& mesh, const ref<MaterialInstance>& material, const T& instance, u32 binding = 0, u32 group = 1) {
        static_assert(std::is_trivially_copyable_v<T>, "Instance type must be POD");

        const void* ptr = &instance;
        u32 size = sizeof(T);

        TR_CORE_ASSERT(ptr != nullptr, "Instance pointer is null!");
        TR_CORE_ASSERT(size > 0, "Instance size must be greater than zero!");
        
        this->submit(mesh, material, ptr, size, binding, group);
    }


    void submit(
        const ref<Mesh>& mesh,
        const ref<MaterialInstance>& material,
        const void* instance,
        u32 size,
        u32 binding = 0,
        u32 group = 1
    );

    void add_dependency(RenderPass* pass);
    const std::vector<RenderPass*>& get_dependencies() const;

    inline wgpu::RenderPassEncoder get_encoder() const { return m_pass_encoder; }
    inline const std::string_view& get_name() const { return m_desc.name; }

private:
    struct DrawBatch {
        ref<Mesh> mesh;
        ref<MaterialInstance> material;

        std::vector<u8> instance_data;
        u32 instance_stride = 0;
        u32 instance_count = 0;
        u32 binding = 0;
        u32 group = 1;

        wgpu::Buffer instance_buffer;
        u64 buffer_capacity = 0;
    };

    RenderPassDesc m_desc;
    CommandQueue& m_queue;

    wgpu::RenderPassEncoder m_pass_encoder = nullptr;
    std::vector<DrawBatch> m_batches;

    std::vector<RenderPass*> m_dependencies;
};


} // namespace eng 
