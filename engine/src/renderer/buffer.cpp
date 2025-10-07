
#include "eng/core/logger.h"
#include "engpch.h"

#include "eng/renderer/context/context_utils.h"
#include "eng/renderer/context/context.h"
#include "eng/renderer/context/command_queue.h"
#include "eng/renderer/buffer.h"
#include "eng/renderer/helpers/string.h"
#include "eng/renderer/helpers/user_data.h"


namespace eng {

void fetch_buffer_data_sync(wgpu::Instance instance, wgpu::Buffer bufferB, std::function<void (const void *)> process_buffer_data) {
	// Context passed to `onBufferBMapped` through theuserdata pointer:

    request_userdata<bool> user_data;

    auto on_buffer_mapped = [](
        wgpu::MapAsyncStatus status,
        wgpu::StringView message,
        eng::request_userdata<bool>* user_data
    ) {
        user_data->request_ended = true;
        if (status == wgpu::MapAsyncStatus::Success) {
            user_data->result = true;
        } else {
            ENG_CORE_ERROR("Could not map buffer B! Status: {}, message: {}", (u32)status, message.data);
        }
    };

    bufferB.MapAsync(
        wgpu::MapMode::Read,
		0, // offset
		WGPU_WHOLE_MAP_SIZE,
        wgpu::CallbackMode::AllowProcessEvents,
        on_buffer_mapped,
        &user_data
    );

    // wgpuBufferMapAsync(
	// 	bufferB,
	// 	wgpu::MapMode::Read,
	// 	0, // offset
	// 	WGPU_WHOLE_MAP_SIZE,
	// 	callback_info
	// );

    instance.ProcessEvents();
	while (!user_data.request_ended) {
		sleep_for_ms(200);
		instance.ProcessEvents();
	}
	
	if (user_data.result) {
        const void* buffer_data = bufferB.GetConstMappedRange(0, WGPU_WHOLE_MAP_SIZE);
		process_buffer_data(buffer_data);
	}

}

wgpu::Buffer Buffer::create(wgpu::Device device, wgpu::Queue queue, const void *data, size_t size, wgpu::BufferUsage usage, const char* label) {
    wgpu::BufferDescriptor desc = {};
    desc.size = size;
    desc.usage = usage;
    desc.mappedAtCreation = false;
    desc.label = label ? label : "Unnamed Buffer";

    wgpu::Buffer buffer = device.CreateBuffer(&desc);

    if (data) {
        queue.WriteBuffer(buffer, 0, data, size);
    }

    return buffer;
}

UniformBuffer Buffer::create_uniform_buffer(Context& ctx, const void* data, u64 size, u32 binding, const char* label) {
    wgpu::Device device = ctx.get_native_device();
    wgpu::Queue queue = ctx.get_queue()->get_native_queue();

    UniformBuffer ub;
    ub.size = size;
    ub.binding = binding;
    ub.label = label;
    ub.buffer = Buffer::create(
        device, 
        queue, 
        data, 
        size, 
        wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst, 
        label
    );

    return ub;
}

VertexBuffer Buffer::create_vertex_buffer(Context& ctx, const void* data, u64 size, u32 slot, const char* label) {
    wgpu::Device device = ctx.get_native_device();
    wgpu::Queue queue = ctx.get_queue()->get_native_queue();

    VertexBuffer vb;
    vb.size = size;
    vb.slot = slot;
    vb.label = label;
    vb.buffer = Buffer::create(
        device, 
        queue, 
        data, 
        size, 
        wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst, 
        label
    );

    return vb;
}

IndexBuffer Buffer::create_index_buffer(Context& ctx, const void* data, u64 size, wgpu::IndexFormat format, const char* label) {
    wgpu::Device device = ctx.get_native_device();
    wgpu::Queue queue = ctx.get_queue()->get_native_queue();

    IndexBuffer ib;
    ib.size = size;
    ib.format = format;
    ib.label = label;
    ib.buffer = Buffer::create(
        device, 
        queue, 
        data, 
        size, 
        wgpu::BufferUsage::Index | wgpu::BufferUsage::CopyDst, 
        label
    );

    return ib;
}

StorageBuffer Buffer::create_storage_buffer(Context& ctx, const void* data, u64 size, u32 binding, const char* label) {
    wgpu::Device device = ctx.get_native_device();
    wgpu::Queue queue = ctx.get_queue()->get_native_queue();

    StorageBuffer sb;
    sb.size = size;
    sb.binding = binding;
    sb.label = label;
    sb.buffer = Buffer::create(
        device,
        queue,
        data,
        size,
        wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopyDst,
        label
    );

    return sb;
}


void Buffer::example(wgpu::Instance instance, wgpu::Device device, wgpu::Queue queue) {
    wgpu::BufferDescriptor buffer_desc = {};
    buffer_desc.nextInChain = nullptr;
    buffer_desc.label = "Example Buffer";
    buffer_desc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::CopySrc;
    buffer_desc.size = 16;
    buffer_desc.mappedAtCreation = false;
    
    wgpu::Buffer buffer1 = device.CreateBuffer(&buffer_desc);

    buffer_desc.label = "Example Buffer 2";
    buffer_desc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::MapRead;
    wgpu::Buffer buffer2 = device.CreateBuffer(&buffer_desc);

    std::vector<uint8_t> numbers(16);
    for (uint8_t i = 0; i < 16; ++i) numbers[i] = i;

    queue.WriteBuffer(buffer1, 0, numbers.data(), numbers.size());

    wgpu::CommandEncoder encoder = device.CreateCommandEncoder(nullptr);

    encoder.CopyBufferToBuffer(buffer1, 0, buffer2, 0, 16);

    wgpu::CommandBuffer command = encoder.Finish();
    queue.Submit(1, &command);

    fetch_buffer_data_sync(
        instance,  // assumes you have a helper or store instance
        buffer2,
        [](const void* data) {
            auto* mapped = static_cast<const uint8_t*>(data);
            ENG_CORE_INFO("Mapped Buffer Data:");
            for (size_t i = 0; i < 16; ++i) {
                ENG_CORE_INFO("  [{}] = {}", i, static_cast<i32>(mapped[i]));
            }
        }
    );
}

}
