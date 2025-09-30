// #pragma once

// #include "eng/helpers/user_data.h"

// namespace eng {

// #define FOREACH_SYNC_REQUEST(cb_name, StatusType, HandleType, SuccessEnum)  \
//     cb_name##_callback,                                                     \
//     void cb_name##_callback(StatusType status, HandleType handle,           \
//                             wgpu::StringView message,                         \
//                             void* userdata1, void* userdata2)

// #define DEFINE_SYNC_REQUEST_CALLBACK(fn, StatusType, HandleType, SuccessEnum)           \
// static void fn##_callback(                                                              \
//     StatusType       status,                                                            \
//     HandleType       handle,                                                            \
//     wgpu::StringView   message,                                                           \
//     void*            userdata1,                                                         \
//     void*            /*userdata2*/)                                                     \
// {                                                                                       \
//     using UD = request_userdata<HandleType>;                                            \
//     auto* ud = reinterpret_cast<UD*>(userdata1);                                        \
//     if (status == SuccessEnum) {                                                        \
//         ud->result = handle;                                                            \
//     } else {                                                                            \
//         std::string_view msg{message.data, message.length};                             \
//         FNC_CORE_ERROR("{} failed: {}", #fn, msg);                                       \
//     }                                                                                   \
//     ud->request_ended = true;                                                           \
// }

// #define DEFINE_ERROR_CALLBACK(fn, ReasonType, EnumSuffix)                        \
// static void fn(                                                                  \
//     const wgpu::Device* device,                                                  \
//     ReasonType        reason,                                                    \
//     wgpu::StringView    message)                                                 \
// {                                                                                \
//     std::string msg{message.data, message.length};                               \
//     if (msg.empty()) {                                                           \
//         FNC_CORE_ERROR("{}: {}", #fn, reason);                                    \
//     } else {                                                                     \
//         FNC_CORE_ERROR("{}: {}: {}", #fn, reason, msg);                           \
//     }                                                                            \
// }

// DEFINE_SYNC_REQUEST_CALLBACK(request_adapter, wgpu::RequestAdapterStatus, wgpu::Adapter, wgpu::RequestAdapterStatus::Success)
// DEFINE_SYNC_REQUEST_CALLBACK(request_device,  wgpu::RequestDeviceStatus,  wgpu::Device,  wgpu::RequestDeviceStatus::Success)

// // Define them:
// // DEFINE_ERROR_CALLBACK(on_device_lost,       wgpu::DeviceLostReason,     ::Success)
// // DEFINE_ERROR_CALLBACK(on_uncaptured_error,  wgpu::ErrorType,            ::Success)

// #undef DEFINE_ERROR_CALLBACK

// #define FOREACH_WGPU_FEATURE_NAME(X)                            \
//     X(DepthClipControl)                                         \
//     X(Depth32FloatStencil8)                                     \
//     X(TimestampQuery)                                           \
//     X(TextureCompressionBC)                                     \
//     X(TextureCompressionBCSliced3D)                             \
//     X(TextureCompressionETC2)                                   \
//     X(TextureCompressionASTC)                                   \
//     X(TextureCompressionASTCSliced3D)                           \
//     X(IndirectFirstInstance)                                    \
//     X(ShaderF16)                                                \
//     X(RG11B10UfloatRenderable)                                  \
//     X(BGRA8UnormStorage)                                        \
//     X(Float32Filterable)                                        \
//     X(Float32Blendable)                                         \
//     X(ClipDistances)                                            \
//     X(DualSourceBlending)                                       \
//     X(Subgroups)                                                \
//     X(CoreFeaturesAndLimits)                                    \
//     X(DawnInternalUsages)                                       \
//     X(DawnMultiPlanarFormats)                                   \
//     X(DawnNative)                                               \
//     X(ChromiumExperimentalTimestampQueryInsidePasses)           \
//     X(ImplicitDeviceSynchronization)                            \
//     X(TransientAttachments)                                     \
//     X(MSAARenderToSingleSampled)                                \
//     X(D3D11MultithreadProtected)                                \
//     X(ANGLETextureSharing)                                      \
//     X(PixelLocalStorageCoherent)                                \
//     X(PixelLocalStorageNonCoherent)                             \
//     X(Unorm16TextureFormats)                                    \
//     X(Snorm16TextureFormats)                                    \
//     X(MultiPlanarFormatExtendedUsages)                          \
//     X(MultiPlanarFormatP010)                                    \
//     X(HostMappedPointer)                                        \
//     X(MultiPlanarRenderTargets)                                 \
//     X(MultiPlanarFormatNv12a)                                   \
//     X(FramebufferFetch)                                         \
//     X(BufferMapExtendedUsages)                                  \
//     X(AdapterPropertiesMemoryHeaps)                             \
//     X(AdapterPropertiesD3D)                                     \
//     X(AdapterPropertiesVk)                                      \
//     X(R8UnormStorage)                                           \
//     X(DawnFormatCapabilities)                                   \
//     X(DawnDrmFormatCapabilities)                                \
//     X(Norm16TextureFormats)                                     \
//     X(MultiPlanarFormatNv16)                                    \
//     X(MultiPlanarFormatNv24)                                    \
//     X(MultiPlanarFormatP210)                                    \
//     X(MultiPlanarFormatP410)                                    \
//     X(SharedTextureMemoryVkDedicatedAllocation)                 \
//     X(SharedTextureMemoryAHardwareBuffer)                       \
//     X(SharedTextureMemoryDmaBuf)                                \
//     X(SharedTextureMemoryOpaqueFD)                              \
//     X(SharedTextureMemoryZirconHandle)                          \
//     X(SharedTextureMemoryDXGISharedHandle)                      \
//     X(SharedTextureMemoryD3D11Texture2D)                        \
//     X(SharedTextureMemoryIOSurface)                             \
//     X(SharedTextureMemoryEGLImage)                              \
//     X(SharedFenceVkSemaphoreOpaqueFD)                           \
//     X(SharedFenceSyncFD)                                        \
//     X(SharedFenceVkSemaphoreZirconHandle)                       \
//     X(SharedFenceDXGISharedHandle)                              \
//     X(SharedFenceMTLSharedEvent)                                \
//     X(SharedBufferMemoryD3D12Resource)                          \
//     X(StaticSamplers)                                           \
//     X(YCbCrVulkanSamplers)                                      \
//     X(ShaderModuleCompilationOptions)                           \
//     X(DawnLoadResolveTexture)                                   \
//     X(DawnPartialLoadResolveTexture)                            \
//     X(MultiDrawIndirect)                                        \
//     X(DawnTexelCopyBufferRowAlignment)                          \
//     X(FlexibleTextureViews)                                     \
//     X(ChromiumExperimentalSubgroupMatrix)                       \
//     X(SharedFenceEGLSync)                                       \
//     X(DawnDeviceAllocatorControl)                               \
//     X(Force32)

// #define FOREACH_WGPU_BACKEND_TYPE(X)    \
//     X(Undefined)                        \
//     X(Null)                             \
//     X(WebGPU)                           \
//     X(D3D11)                            \
//     X(D3D12)                            \
//     X(Metal)                            \
//     X(Vulkan)                           \
//     X(OpenGL)                           \
//     X(OpenGLES)                         \
//     X(Force32)

// #define FOREACH_WGPU_ADAPTER_TYPE(X)    \
//     X(DiscreteGPU)                      \
//     X(IntegratedGPU)                    \
//     X(CPU)                              \
//     X(Unknown)                          \
//     X(Force32)


// static const char* feature_name_to_string(wgpu::FeatureName f) {
//     switch (f) {
// #define MAKE_CASE(name) \
//         case wgpu::FeatureName_##name: return #name;
//         FOREACH_wgpu::_FEATURE_NAME(MAKE_CASE)
// #undef MAKE_CASE

//         default: {
//             static char buf[32];
//             snprintf(buf, sizeof(buf), "Unknown(0x%X)", (u32) f);
//             return buf;
//         }
//     }
// }

// static const char* backend_type_to_string(wgpu::BackendType f) {
//     switch (f) {
// #define MAKE_CASE(name) \
//         case WGPUBackendType##name: return #name;
//         FOREACH_WGPU_BACKEND_TYPE(MAKE_CASE)
// #undef MAKE_CASE

//         default: {
//             static char buf[32];
//             snprintf(buf, sizeof(buf), "Unknown(0x%X)", (u32) f);
//             return buf;
//         }
//     }
// }

// static const char* adapter_type_to_string(WGPUAdapterType f) {
//     switch (f) {
// #define MAKE_CASE(name) \
//         case WGPUAdapterType_##name: return #name;
//         FOREACH_WGPU_ADAPTER_TYPE(MAKE_CASE)
// #undef MAKE_CASE

//         default: {
//             static char buf[32];
//             snprintf(buf, sizeof(buf), "Unknown(0x%X)", (u32) f);
//             return buf;
//         }
//     }
// }

// #define FOREACH_WGPU_PRESENT_MODE(X) \
//     X(Undefined)                     \
//     X(Fifo)                          \
//     X(FifoRelaxed)                   \
//     X(Immediate)                     \
//     X(Mailbox)                       \
//     X(Force32)

// #define FOREACH_WGPU_COMPOSITE_ALPHA_MODE(X) \
//     X(Auto)                                   \
//     X(Opaque)                                 \
//     X(Premultiplied)                          \
//     X(Unpremultiplied)                        \
//     X(Inherit)                                \
//     X(Force32)

// #define FOREACH_WGPU_TEXTURE_FORMAT(X) \
//     X(Undefined) \
//     X(R8Unorm) X(R8Snorm) X(R8Uint) X(R8Sint) \
//     X(R16Uint) X(R16Sint) X(R16Float) \
//     X(RG8Unorm) X(RG8Snorm) X(RG8Uint) X(RG8Sint) \
//     X(R32Float) X(R32Uint) X(R32Sint) \
//     X(RG16Uint) X(RG16Sint) X(RG16Float) \
//     X(RGBA8Unorm) X(RGBA8UnormSrgb) X(RGBA8Snorm) X(RGBA8Uint) X(RGBA8Sint) \
//     X(BGRA8Unorm) X(BGRA8UnormSrgb) \
//     X(RGB10A2Uint) X(RGB10A2Unorm) X(RG11B10Ufloat) X(RGB9E5Ufloat) \
//     X(RG32Float) X(RG32Uint) X(RG32Sint) \
//     X(RGBA16Uint) X(RGBA16Sint) X(RGBA16Float) \
//     X(RGBA32Float) X(RGBA32Uint) X(RGBA32Sint) \
//     X(Stencil8) X(Depth16Unorm) X(Depth24Plus) X(Depth24PlusStencil8) X(Depth32Float) X(Depth32FloatStencil8) \
//     X(BC1RGBAUnorm) X(BC1RGBAUnormSrgb) X(BC2RGBAUnorm) X(BC2RGBAUnormSrgb) X(BC3RGBAUnorm) X(BC3RGBAUnormSrgb) \
//     X(BC4RUnorm) X(BC4RSnorm) X(BC5RGUnorm) X(BC5RGSnorm) \
//     X(BC6HRGBUfloat) X(BC6HRGBFloat) X(BC7RGBAUnorm) X(BC7RGBAUnormSrgb) \
//     X(ETC2RGB8Unorm) X(ETC2RGB8UnormSrgb) X(ETC2RGB8A1Unorm) X(ETC2RGB8A1UnormSrgb) \
//     X(ETC2RGBA8Unorm) X(ETC2RGBA8UnormSrgb) \
//     X(EACR11Unorm) X(EACR11Snorm) X(EACRG11Unorm) X(EACRG11Snorm) \
//     X(ASTC4x4Unorm) X(ASTC4x4UnormSrgb) X(ASTC5x4Unorm) X(ASTC5x4UnormSrgb) \
//     X(ASTC5x5Unorm) X(ASTC5x5UnormSrgb) X(ASTC6x5Unorm) X(ASTC6x5UnormSrgb) \
//     X(ASTC6x6Unorm) X(ASTC6x6UnormSrgb) X(ASTC8x5Unorm) X(ASTC8x5UnormSrgb) \
//     X(ASTC8x6Unorm) X(ASTC8x6UnormSrgb) X(ASTC8x8Unorm) X(ASTC8x8UnormSrgb) \
//     X(ASTC10x5Unorm) X(ASTC10x5UnormSrgb) X(ASTC10x6Unorm) X(ASTC10x6UnormSrgb) \
//     X(ASTC10x8Unorm) X(ASTC10x8UnormSrgb) X(ASTC10x10Unorm) X(ASTC10x10UnormSrgb) \
//     X(ASTC12x10Unorm) X(ASTC12x10UnormSrgb) X(ASTC12x12Unorm) X(ASTC12x12UnormSrgb) \
//     X(R16Unorm) X(RG16Unorm) X(RGBA16Unorm) \
//     X(R16Snorm) X(RG16Snorm) X(RGBA16Snorm) \
//     X(R8BG8Biplanar420Unorm) X(R10X6BG10X6Biplanar420Unorm) X(R8BG8A8Triplanar420Unorm) \
//     X(R8BG8Biplanar422Unorm) X(R8BG8Biplanar444Unorm) \
//     X(R10X6BG10X6Biplanar422Unorm) X(R10X6BG10X6Biplanar444Unorm) \
//     X(External) X(Force32)

// static const char* present_mode_to_string(WGPUPresentMode mode) {
//     switch (mode) {
// #define MAKE_CASE(name) case WGPUPresentMode_##name: return #name;
//         FOREACH_WGPU_PRESENT_MODE(MAKE_CASE)
// #undef MAKE_CASE
//         default: {
//             static char buf[32];
//             snprintf(buf, sizeof(buf), "Unknown(0x%X)", static_cast<u32>(mode));
//             return buf;
//         }
//     }
// }

// static const char* alpha_mode_to_string(WGPUCompositeAlphaMode mode) {
//     switch (mode) {
// #define MAKE_CASE(name) case WGPUCompositeAlphaMode_##name: return #name;
//         FOREACH_WGPU_COMPOSITE_ALPHA_MODE(MAKE_CASE)
// #undef MAKE_CASE
//         default: {
//             static char buf[32];
//             snprintf(buf, sizeof(buf), "Unknown(0x%X)", static_cast<u32>(mode));
//             return buf;
//         }
//     }
// }

// static const char* texture_format_to_string(WGPUTextureFormat fmt) {
//     switch (fmt) {
// #define MAKE_CASE(name) case WGPUTextureFormat_##name: return #name;
//         FOREACH_WGPU_TEXTURE_FORMAT(MAKE_CASE)
// #undef MAKE_CASE
//         default: {
//             static char buf[64];
//             snprintf(buf, sizeof(buf), "Unknown(0x%X)", static_cast<u32>(fmt));
//             return buf;
//         }
//     }
// }




// }
