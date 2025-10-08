

#include "eng/debug/profiler.h"

#include "eng/renderer/context/context.h"
#include "eng/renderer/context/context_utils.h"
#include "eng/renderer/context/command_queue.h"
#include "eng/renderer/context/macros.h"
#include "eng/platform/window.h"

#include "eng/renderer/helpers/user_data.h"
#include "eng/renderer/helpers/string.h"

#include "eng/renderer/helpers/webgpu_fmt_formatters.h"

namespace eng {

GraphicsContext::GraphicsContext(const ContextProps& props)
    : m_props(props)
{}

GraphicsContext::~GraphicsContext() {}


void on_device_lost(const wgpu::Device& device, wgpu::DeviceLostReason reason, wgpu::StringView message) {
    std::string msg{message.data, message.length};
    ENG_CORE_ERROR("on_device_lost: {}: {}", (i32) reason, msg);
}

void on_uncaptured_error(const wgpu::Device& device, wgpu::ErrorType type, wgpu::StringView message) {
    std::string msg{message.data, message.length};
    ENG_CORE_ERROR("on_uncaptured_error: {}: {}", (i32) type, msg);
}



void GraphicsContext::init(Window* window_handle) {
	ENG_CORE_ASSERT(window_handle, "Window handle is null!");

	m_window_handle = window_handle;
	wgpu::InstanceDescriptor desc = {};
	desc.nextInChain = nullptr;

	// Create instance
#ifdef WEBGPU_BACKEND_EMSCRIPTEN
	m_instance = wgpuCreateInstance(nullptr);
#else //  WEBGPU_BACKEND_EMSCRIPTEN
	m_instance = wgpu::CreateInstance(&desc);
#endif //  WEBGPU_BACKEND_EMSCRIPTEN

	ENG_CORE_ASSERT(m_instance, "WebGPU instance not yet created");

    m_surface = window_handle->get_surface(m_instance);

	wgpu::RequestAdapterOptions adapter_opts = {};
	adapter_opts.nextInChain = nullptr;
    adapter_opts.compatibleSurface = m_surface;
	wgpu::Adapter adapter = request_adapter_sync(m_instance, &adapter_opts);

	inspect_adapter(adapter);

    wgpu::DeviceDescriptor device_desc = {};
    device_desc.label = "FinancioDevice";
    device_desc.defaultQueue.label = "MainQueue";

    request_userdata<u32> data;

    device_desc.SetDeviceLostCallback(wgpu::CallbackMode::AllowSpontaneous, &on_device_lost);

    device_desc.SetUncapturedErrorCallback(&on_uncaptured_error);

	m_device = request_device_sync(adapter, &device_desc);

    inspect_device(m_device);

	m_queue = CommandQueue::create();
    m_queue->init(m_device);

	m_surface_format = inspect_surface_capabilities(m_surface, adapter);
	configure_surface(m_surface_format);
}

void GraphicsContext::configure_surface(wgpu::TextureFormat preferred_format) {
	ENG_CORE_INFO("Configuring swap chain...");

    auto [fb_width, fb_height] = m_window_handle->get_framebuffer_size();

	wgpu::SurfaceConfiguration config = {};
    config.device       = m_device;
    config.format       = preferred_format;
    config.usage        = wgpu::TextureUsage::RenderAttachment;
    config.width        = fb_width;
    config.height       = fb_height;
    config.presentMode  = wgpu::PresentMode::Fifo;
    config.alphaMode    = wgpu::CompositeAlphaMode::Auto;
    config.viewFormatCount = 0;
    config.viewFormats     = nullptr;

    m_surface.Configure(&config);

    ENG_CORE_INFO("Surface configured: {}x{} @ format {}", fb_width, fb_height, (i32)preferred_format);
}

wgpu::TextureView GraphicsContext::get_next_surface_view() {
    PROFILE_FUNCTION();

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    wgpu::SurfaceTexture surface_texture = {};

    {
        PROFILE_SCOPE("Getting Current Texture");
        m_surface.GetCurrentTexture(&surface_texture);
    }

	if (surface_texture.status != wgpu::SurfaceGetCurrentTextureStatus::SuccessOptimal &&
		surface_texture.status != wgpu::SurfaceGetCurrentTextureStatus::SuccessSuboptimal) {
		ENG_CORE_ERROR("Failed to acquire surface texture! Status = {}", surface_texture.status);
		return nullptr;
	}
	if (surface_texture.status == wgpu::SurfaceGetCurrentTextureStatus::SuccessSuboptimal) {
		ENG_CORE_WARN("Acquired surface texture is suboptimal. You should reconfigure the surface.");
	}

	wgpu::TextureViewDescriptor view_desc = {};
    view_desc.nextInChain = nullptr;
    view_desc.label = "Surface texture view";
    view_desc.dimension = wgpu::TextureViewDimension::e2D;
    view_desc.format = surface_texture.texture.GetFormat();
    view_desc.baseMipLevel = 0;
    view_desc.mipLevelCount = 1;
    view_desc.baseArrayLayer = 0;
    view_desc.arrayLayerCount = 1;
    view_desc.aspect = wgpu::TextureAspect::All;

    {
        PROFILE_SCOPE("Creating Texture View");
        wgpu::TextureView texture_view = surface_texture.texture.CreateView(&view_desc);
        return texture_view;

    }

    // return texture_view;

}

std::pair<u32, u32> GraphicsContext::get_framebuffer_size() { 
    return m_window_handle->get_framebuffer_size(); 
}


void GraphicsContext::swap_buffers() {
    PROFILE_FUNCTION();

	// Present logic would go here
	// In a real app, you'd acquire the next texture and render to it
	m_surface.Present(); // Add this here
}

scope<Context> GraphicsContext::create(const ContextProps& props) {
    return create_scope<GraphicsContext>(props);
}

}
