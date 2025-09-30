// error_scope_utils.h

#pragma once

#include "eng/core/logger.h"

#define WGPU_PUSH_ERROR_SCOPE(dev) wgpuDevicePushErrorScope((dev), WGPUErrorFilter_Validation)

#define WGPU_POP_ERROR_SCOPE_LOG(dev)                                    \
    do {                                                                 \
        WGPUPopErrorScopeCallbackInfo _cb = WGPU_POP_ERROR_SCOPE_CALLBACK_INFO_INIT; \
        _cb.mode = WGPUCallbackMode_AllowSpontaneous;                    \
        _cb.callback = +[](WGPUPopErrorScopeStatus status, WGPUErrorType type, WGPUStringView msg, void*, void*) { \
            if (status == WGPUPopErrorScopeStatus_Success && type == WGPUErrorType_NoError) { \
                TR_CORE_TRACE("WGPU: No error from scope.");             \
            } else {                                                     \
                TR_CORE_ERROR("WGPU error (status={}, type={}): {}",    \
                    (int)status, (int)type, std::string_view(msg.data, msg.length)); \
            }                                                            \
        };                                                               \
        wgpuDevicePopErrorScope((dev), &_cb);                            \
    } while (0)

    // You must define: eng::request_userdata<bool> my_result;
#define WGPU_POP_ERROR_SCOPE_CAPTURE_BOOL(dev, ptr_result)              \
    do {                                                                \
        wgpu::PopErrorScopeCallbackInfo _cb = {}; \
        _cb.mode = wgpu::CallbackMode::AllowSpontaneous;                   \
        _cb.userdata1 = (ptr_result);                                   \
        _cb.callback = +[](wgpu::PopErrorScopeStatus status,              \
                           WGPUErrorType type,                          \
                           WGPUStringView msg,                          \
                           void* user1, void*) {                         \
            auto* result = static_cast<eng::request_userdata<bool>*>(user1); \
            result->request_ended = true;                               \
            if (status == WGPUPopErrorScopeStatus_Success && type == WGPUErrorType_NoError) { \
                result->result = true;                                  \
                TR_CORE_TRACE("WGPU: Shader compiled successfully.");   \
            } else {                                                    \
                result->result = false;                                 \
                TR_CORE_ERROR("WGPU validation failed: {}",             \
                    std::string_view(msg.data, msg.length));           \
            }                                                           \
        };                                                              \
        dev.PopErrorScope(_cb);                                         \
    } while (0)
