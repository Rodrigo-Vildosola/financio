#include "eng/renderer/shader.h"
#include "eng/helpers/error.h"
#include "eng/helpers/string.h"
#include "eng/helpers/user_data.h"

namespace eng {

std::filesystem::path get_asset_path(const std::string& relative_path) {
    return std::filesystem::path(ENGINE_ASSETS_DIR) / relative_path;
}

std::string read_file_as_string(const std::string& relative_path) {
    std::filesystem::path full_path = get_asset_path(relative_path);
    std::ifstream file(full_path);
    
    if (!file.is_open()) {
        ENG_CORE_ERROR("Failed to open file: {}", full_path.string());
        return {};
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

Shader::~Shader() {}

Shader Shader::create_from_wgsl(Context& ctx, std::string_view source, std::string_view label) {
    const auto& device = ctx.get_native_device();

    // 1 Prepare WGSL descriptor
    wgpu::ShaderSourceWGSL wgsl_desc = {};
    wgsl_desc.code = to_wgpu_string_view(source);

    wgpu::ShaderModuleDescriptor desc = {};
    desc.nextInChain = &wgsl_desc;
    desc.label       = to_wgpu_string_view(label);

    // 2 Error‐scope around compilation
    request_userdata<bool> compile_ok;

    // WGPU_PUSH_ERROR_SCOPE(device);
    device.PushErrorScope(wgpu::ErrorFilter::Validation);

    wgpu::ShaderModule module = device.CreateShaderModule(&desc);

    device.PopErrorScope(
        wgpu::CallbackMode::AllowSpontaneous, 
        [&compile_ok](wgpu::PopErrorScopeStatus status, wgpu::ErrorType error_type, wgpu::StringView message) {
            compile_ok.request_ended = true;
            if (status == wgpu::PopErrorScopeStatus::Success && error_type == wgpu::ErrorType::NoError) {
                compile_ok.result = true;
                ENG_CORE_TRACE("WGPU: Shader compiled successfully.");
            } else {
                compile_ok.result = false;
                ENG_CORE_ERROR("WGPU validation failed: {}", std::string_view(message.data, message.length));
            }
        }
    );
    
    // WGPU_POP_ERROR_SCOPE_CAPTURE_BOOL(device, &compile_ok);

    if (!compile_ok.result) {
        ENG_CORE_ERROR("Shader “{}” failed to compile", label);
        // You can choose to throw, or continue with a null module:
    }

    return Shader(module, std::string(label));
}


Shader Shader::from_file(Context& ctx, const std::string& path, std::string label) {
    std::string source = read_file_as_string(path);

    Shader shader = create_from_wgsl(ctx, source, label);
    shader.m_source = std::move(source);
    return shader;
}

Shader::Shader(Shader&& other) noexcept
    : m_module(other.m_module),
      m_source(std::move(other.m_source)),
      label(std::move(other.label)),
      vertex_entry(std::move(other.vertex_entry)),
      fragment_entry(std::move(other.fragment_entry)) {
    other.m_module = nullptr;
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        m_module = other.m_module;
        m_source = std::move(other.m_source);
        label = std::move(other.label);
        vertex_entry = std::move(other.vertex_entry);
        fragment_entry = std::move(other.fragment_entry);
        other.m_module = nullptr;
    }
    return *this;
}

} // namespace eng
