#include "eng/renderer/context/context_utils.h"
#include "eng/renderer/context/macros.h"
#include "eng/debug/profiler.h"
#include "eng/renderer/helpers/user_data.h"
#include "eng/renderer/helpers/webgpu_fmt_formatters.h"


namespace eng {


void wgpu_poll_events(wgpu::Device device, bool yield_to_browser) {
    PROFILE_FUNCTION();

    #if defined(WEBGPU_BACKEND_DAWN)
        device.Tick();
    #elif defined(WEBGPU_BACKEND_wgpu::)
        wgpuDevicePoll(device, false, nullptr);
    #elif defined(WEBGPU_BACKEND_EMSCRIPTEN)
        if (yield_to_browser) {
            emscripten_sleep(100);
        }
    #endif
}


void sleep_for_ms(unsigned int milliseconds) {
    #ifdef __EMSCRIPTEN__
        emscripten_sleep(milliseconds);
    #else
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    #endif
}

wgpu::Adapter request_adapter_sync(wgpu::Instance instance, wgpu::RequestAdapterOptions const* options) {
	// A simple structure holding the local information shared with the
	// onAdapterrequest_ended callback.
	request_userdata<wgpu::Adapter> user_data;

	// Callback called by wgpuInstanceRequestAdapter when the request returns
	// This is a C++ lambda function, but could be any function defined in the
	// global scope. It must be non-capturing (the brackets [] are empty) so
	// that it behaves like a regular C function pointer, which is what
	// wgpuInstanceRequestAdapter expects (WebGPU being a C API). The workaround
	// is to convey what we want to capture through the pUserData pointer,
	// provided as the last argument of wgpuInstanceRequestAdapter and received
	// by the callback as its last argument.


    instance.RequestAdapter(
        options,
        wgpu::CallbackMode::AllowProcessEvents,
        [&user_data](wgpu::RequestAdapterStatus status, wgpu::Adapter adapter, const char* message) {
            if (status == wgpu::RequestAdapterStatus::Success) {
                user_data.result = adapter;
            } else {
                ENG_CORE_ERROR("RequestAdapter failed: {}", message ? message : "No message");
            }
            user_data.request_ended = true;
        });
    
    instance.ProcessEvents();
    while (!user_data.request_ended) {
        sleep_for_ms(200); // 200ms
        instance.ProcessEvents();
    }

	ENG_CORE_ASSERT(user_data.request_ended, "Request for adapter has not ended");

    return user_data.result;

}

wgpu::Device request_device_sync(wgpu::Adapter adapter, wgpu::DeviceDescriptor const* descriptor) {

    request_userdata<wgpu::Device> user_data;

    adapter.RequestDevice(
        descriptor,
        wgpu::CallbackMode::AllowProcessEvents,
        [&user_data](wgpu::RequestDeviceStatus status, wgpu::Device device, const char* message) {
            if (status == wgpu::RequestDeviceStatus::Success) {
                user_data.result = device;
            } else {
                ENG_CORE_ERROR("RequestAdapter failed: {}", message ? message : "No message");
            }
            user_data.request_ended = true;
        });

    wgpu::Instance instance = adapter.GetInstance();

    instance.ProcessEvents();
    while (!user_data.request_ended) {
        sleep_for_ms(200); // 200ms
        instance.ProcessEvents();
    }

	ENG_CORE_ASSERT(user_data.request_ended, "Request for device has not ended");

    return user_data.result;
}


void inspect_adapter(wgpu::Adapter adapter) {
#ifndef __EMSCRIPTEN__
	wgpu::Limits supported_limits = {};
	supported_limits.nextInChain = nullptr;

#ifdef WEBGPU_BACKEND_DAWN
    bool success = adapter.GetLimits(&supported_limits) == wgpu::Status::Success;
#else
	bool success = adapter.GetLimits(&supported_limits);
#endif
	if (success) {
		ENG_FILE_INFO("Adapter limits:");
		ENG_FILE_TRACE("  maxTextureDimension1D   = {}", supported_limits.maxTextureDimension1D);
		ENG_FILE_TRACE("  maxTextureDimension2D   = {}", supported_limits.maxTextureDimension2D);
		ENG_FILE_TRACE("  maxTextureDimension3D   = {}", supported_limits.maxTextureDimension3D);
		ENG_FILE_TRACE("  maxTextureArrayLayers   = {}", supported_limits.maxTextureArrayLayers);
	} else {
		ENG_CORE_WARN("Could not query adapter limits");
	}
#endif // NOT __EMSCRIPTEN__

    // 2) Features
    {
        wgpu::SupportedFeatures feats = {};
        adapter.GetFeatures(&feats);

        ENG_CORE_INFO("Adapter features (count={}):", feats.featureCount);
        for (u32 i = 0; i < feats.featureCount; ++i) {
            auto f = feats.features[i];
            // ENG_FILE_TRACE("  - {}", f);
        }
    }

	// 3) Properties
    {
        wgpu::AdapterInfo info = {};
        adapter.GetInfo(&info);

        ENG_CORE_INFO("Adapter properties:");
        ENG_CORE_TRACE("  vendorID           = {}", info.vendorID);
        if (info.vendor.data)
            ENG_CORE_TRACE("  vendor             = {}", 
                std::string(info.vendor.data, info.vendor.length));
        if (info.architecture.data)
            ENG_CORE_TRACE("  architecture       = {}", 
                std::string(info.architecture.data, info.architecture.length));
        ENG_FILE_TRACE("  deviceID           = {}", info.deviceID);
        if (info.device.data)
            ENG_CORE_TRACE("  device             = {}", 
                std::string(info.device.data, info.device.length));
        if (info.description.data)
            ENG_CORE_TRACE("  description        = {}", 
                std::string(info.description.data, info.description.length));
        ENG_FILE_TRACE("  subgroup sizes     = {} .. {}", 
            info.subgroupMinSize, info.subgroupMaxSize);
        ENG_CORE_TRACE("  adapterType        = {}", info.adapterType);
        ENG_CORE_TRACE("  backendType        = {}", info.backendType);
    }

}

void inspect_device(wgpu::Device device) {
#ifndef __EMSCRIPTEN__
    // 1) Features
    {
        wgpu::SupportedFeatures feats = {};
        device.GetFeatures(&feats);

        ENG_CORE_INFO("Device features (count={}):", feats.featureCount);
        for (u32 i = 0; i < feats.featureCount; ++i) {
            auto f = feats.features[i];
            ENG_CORE_TRACE("  - {}", f);
        }
    }

    // 2) Limits
    {
        wgpu::Limits limits = {};

    #ifdef WEBGPU_BACKEND_DAWN
        bool success = device.GetLimits(&limits) == wgpu::Status::Success;
    #else
        bool success = device.GetLimits(&limits);
    #endif

        if (success) {
            ENG_CORE_INFO("Device limits:");
            ENG_CORE_TRACE("  maxTextureDimension1D   = {}", limits.maxTextureDimension1D);
            ENG_CORE_TRACE("  maxTextureDimension2D   = {}", limits.maxTextureDimension2D);
            ENG_CORE_TRACE("  maxTextureDimension3D   = {}", limits.maxTextureDimension3D);
            ENG_CORE_TRACE("  maxTextureArrayLayers   = {}", limits.maxTextureArrayLayers);
            // extra device limits you might care about:
            ENG_CORE_TRACE("  maxComputeWorkgroupSize = {} x {} x {}",
                limits.maxComputeWorkgroupSizeX,
                limits.maxComputeWorkgroupSizeY,
                limits.maxComputeWorkgroupSizeZ);
        } else {
            ENG_CORE_WARN("Could not query device limits");
        }
    }
#endif
}

wgpu::TextureFormat inspect_surface_capabilities(wgpu::Surface surface, wgpu::Adapter adapter) {
    ENG_CORE_INFO("Inspecting surface capabilities...");

    wgpu::SurfaceCapabilities capabilities = {};

    wgpu::Status status = surface.GetCapabilities(adapter, &capabilities);
    if (status != wgpu::Status::Success) {
        ENG_CORE_ERROR("Failed to get surface capabilities (status = {})", status);
        return wgpu::TextureFormat::Undefined;
    }

    ENG_CORE_INFO("Surface Capabilities:");
    ENG_CORE_TRACE("  Texture usage bitmask: {}", capabilities.usages);

    // 1. Formats
    ENG_CORE_TRACE("  Supported formats ({}):", capabilities.formatCount);
    for (size_t i = 0; i < capabilities.formatCount; ++i) {
        ENG_CORE_TRACE("    - {}", capabilities.formats[i]);
    }

    // 2. Present Modes
    ENG_CORE_TRACE("  Supported present modes ({}):", capabilities.presentModeCount);
    for (size_t i = 0; i < capabilities.presentModeCount; ++i) {
        ENG_CORE_TRACE("    - {}", capabilities.presentModes[i]);
    }

    // 3. Alpha Modes
    ENG_CORE_TRACE("  Supported alpha modes ({}):", capabilities.alphaModeCount);
    for (size_t i = 0; i < capabilities.alphaModeCount; ++i) {
        ENG_CORE_TRACE("    - {}", capabilities.alphaModes[i]);
    }

    wgpu::TextureFormat preferred_format = capabilities.formats[0];

    // The first one is always the preferred format!
    return preferred_format;
}


}
