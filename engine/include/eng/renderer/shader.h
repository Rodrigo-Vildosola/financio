#pragma once

#include "engpch.h"
#include "eng/core/context/context.h"

namespace eng {

/// Encapsulates a single shader module (WGSL or SPIR-V).
class Shader {
public:
    /// Read WGSL from `source` (later you can add loadFromFile)
    /// `label` is used for debug markers.
    static Shader create_from_wgsl(Context& ctx, std::string_view source, std::string_view label);

    static Shader from_file(Context& ctx, const std::string& path, std::string label);

    ~Shader();


    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;


    /// The raw wgpu::ShaderModule; for pipeline creation only.
    wgpu::ShaderModule module() const { return m_module; }

    /// Debug labels
    const std::string& source() const { return m_source; }

    std::string      label;
    std::string      vertex_entry = "vs_main";
    std::string      fragment_entry = "fs_main";

private:
    Shader(wgpu::ShaderModule module, std::string label,
           std::string source = {}, std::string vertex_entry = "vs_main", std::string fragment_entry = "fs_main")
        : m_module(module)
        , label(std::move(label))
        , m_source(std::move(source))
        , vertex_entry(std::move(vertex_entry))
        , fragment_entry(std::move(fragment_entry)) {}

    wgpu::ShaderModule m_module;

    std::string      m_source;

};

} // namespace eng
