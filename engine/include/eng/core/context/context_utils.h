#pragma once

#include "engpch.h"

namespace eng {


void wgpu_poll_events(wgpu::Device device, bool yield_to_browser = true);

void sleep_for_ms(unsigned int milliseconds);

/**
 * @brief Utility function to get a WebGPU adapter, so that
 *     wgpu::Adapter adapter = requestAdapterSync(options);
 * is roughly equivalent to
 *     const adapter = await navigator.gpu.requestAdapter(options);
 */
wgpu::Adapter request_adapter_sync(wgpu::Instance instance, wgpu::RequestAdapterOptions const* options);

/**
 * @brief Utility function to get a WebGPU device, so that
 *     wgpu::Device device = requestDeviceSync(adapter, options);
 * is roughly equivalent to
 *     const device = await adapter.requestDevice(descriptor);
 * It is very similar to requestAdapter
 */
wgpu::Device request_device_sync(wgpu::Adapter adapter, wgpu::DeviceDescriptor const* descriptor);


void inspect_adapter(wgpu::Adapter adapter);
void inspect_device(wgpu::Device device);
wgpu::TextureFormat inspect_surface_capabilities(wgpu::Surface surface, wgpu::Adapter adapter);
/**
 * Helper to zero-initialize and populate a wgpu::DeviceDescriptor
 * with sensible defaults, plus optional callbacks.
 */
struct DeviceDescriptorBuilder {
    wgpu::DeviceDescriptor desc = {};
    std::vector<wgpu::FeatureName> feature_buffer;

    DeviceDescriptorBuilder() {
        desc.label                 = nullptr;
        desc.requiredFeatures      = nullptr;
        desc.requiredFeatureCount  = 0;
        desc.requiredLimits        = nullptr;

        desc.defaultQueue.label    = nullptr;

    }

    DeviceDescriptorBuilder& label(std::string_view txt) {
        desc.label = txt.data();
        return *this;
    }

    DeviceDescriptorBuilder& default_queue_label(std::string_view txt) {
        desc.defaultQueue.label = txt.data();
        return *this;
    }


    DeviceDescriptorBuilder& require_features(const std::vector<wgpu::FeatureName>& feats) {
        feature_buffer = feats; // Must keep them alive
        desc.requiredFeatures = feature_buffer.data();
        desc.requiredFeatureCount = (u32) feature_buffer.size();
        return *this;
    }

    // Install your “device lost” callback:
    template <typename F>
    DeviceDescriptorBuilder& on_device_lost(F callback, void* userdata = nullptr) {
        desc.SetDeviceLostCallback(wgpu::CallbackMode::AllowSpontaneous, callback, userdata);
        return *this;
    }

    template <typename F>
    DeviceDescriptorBuilder& on_uncaptured_error(F callback, void* userdata = nullptr) {
        desc.SetUncapturedErrorCallback(callback, userdata);
        return *this;
    }

    // Finally, build the descriptor for passing in:
    const wgpu::DeviceDescriptor& build() {
        return desc;
    }
};



}
