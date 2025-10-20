#pragma once
#include <spdlog/fmt/bundled/format.h>
#include <sstream>
#include <webgpu/webgpu_cpp_print.h>

namespace fmt {

template <>
struct fmt::formatter<wgpu::AdapterType> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::AdapterType value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::AddressMode> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::AddressMode value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::AlphaMode> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::AlphaMode value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::BackendType> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::BackendType value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::BlendFactor> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::BlendFactor value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::BlendOperation> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::BlendOperation value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::BufferBindingType> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::BufferBindingType value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::BufferMapState> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::BufferMapState value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::BufferUsage> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::BufferUsage value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::CallbackMode> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::CallbackMode value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::ColorWriteMask> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::ColorWriteMask value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::CompareFunction> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::CompareFunction value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::CompilationInfoRequestStatus> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::CompilationInfoRequestStatus value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::CompilationMessageType> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::CompilationMessageType value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::CompositeAlphaMode> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::CompositeAlphaMode value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::CreatePipelineAsyncStatus> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::CreatePipelineAsyncStatus value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::CullMode> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::CullMode value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::DeviceLostReason> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::DeviceLostReason value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::ErrorFilter> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::ErrorFilter value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::ErrorType> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::ErrorType value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::ExternalTextureRotation> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::ExternalTextureRotation value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::FeatureLevel> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::FeatureLevel value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::FeatureName> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::FeatureName value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::FilterMode> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::FilterMode value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::FrontFace> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::FrontFace value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::HeapProperty> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::HeapProperty value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::IndexFormat> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::IndexFormat value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::LoadOp> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::LoadOp value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::LoggingType> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::LoggingType value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::MapAsyncStatus> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::MapAsyncStatus value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::MapMode> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::MapMode value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::MipmapFilterMode> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::MipmapFilterMode value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::PopErrorScopeStatus> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::PopErrorScopeStatus value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::PowerPreference> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::PowerPreference value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::PresentMode> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::PresentMode value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::PrimitiveTopology> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::PrimitiveTopology value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::QueryType> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::QueryType value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::QueueWorkDoneStatus> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::QueueWorkDoneStatus value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::RequestAdapterStatus> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::RequestAdapterStatus value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::RequestDeviceStatus> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::RequestDeviceStatus value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::SType> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::SType value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::SamplerBindingType> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::SamplerBindingType value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::ShaderStage> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::ShaderStage value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::SharedFenceType> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::SharedFenceType value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::Status> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::Status value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::StencilOperation> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::StencilOperation value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::StorageTextureAccess> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::StorageTextureAccess value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::StoreOp> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::StoreOp value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::StringView> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::StringView value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::SubgroupMatrixComponentType> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::SubgroupMatrixComponentType value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::SurfaceGetCurrentTextureStatus> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::SurfaceGetCurrentTextureStatus value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::TextureAspect> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::TextureAspect value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::TextureDimension> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::TextureDimension value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::TextureFormat> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::TextureFormat value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::TextureSampleType> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::TextureSampleType value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::TextureUsage> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::TextureUsage value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::TextureViewDimension> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::TextureViewDimension value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::VertexFormat> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::VertexFormat value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::VertexStepMode> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::VertexStepMode value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::WGSLLanguageFeatureName> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::WGSLLanguageFeatureName value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};


template <>
struct fmt::formatter<wgpu::WaitStatus> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(wgpu::WaitStatus value, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};

} // namespace fmt