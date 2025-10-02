#include "financio/root_layer.h"

#include "eng/core/application.h"
#include "eng/debug/profiler.h"
#include "eng/renderer/renderer_api.h"

#include "eng/core/timestep.h"
#include "eng/core/timer.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

using namespace eng;

RootLayer::RootLayer() : Layer("RootLayer") {}

void RootLayer::on_attach() {
    PROFILE_FUNCTION();

    f32 width = Application::get().get_window().get_width();
    f32 height = Application::get().get_window().get_height();
    f32 aspect_ratio = width / height;

    m_shader = RendererAPI::create_shader(
        "shaders/shader.wgsl", 
        "Basic Shader Module"
    );
    m_shader->vertex_entry = "vs_main";
    m_shader->fragment_entry = "fs_main";

    PipelineSpecification spec;
    spec.shader = m_shader;

    VertexBufferLayoutSpec layout;
    layout.stride = sizeof(f32) * 6;
    layout.step_mode = wgpu::VertexStepMode::Vertex;
    layout.attributes = {
        { 0, wgpu::VertexFormat::Float32x3, 0 },
        { 1, wgpu::VertexFormat::Float32x3, sizeof(f32) * 3 },
    };

    VertexBufferLayoutSpec vb = layout;
    spec.vertex_buffers.push_back(vb);

    UniformBufferSpec ubo_spec;
    ubo_spec.binding = 0;
    ubo_spec.group = 0;
    ubo_spec.size = sizeof(UniformBlock);
    ubo_spec.visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;

    spec.uniforms = { ubo_spec };

    StorageBufferSpec sb_spec;
    sb_spec.binding = 0;
    sb_spec.group = 1;
    sb_spec.size = sizeof(InstanceBlock);
    sb_spec.visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;

    spec.storages = { sb_spec };

    ref<Pipeline> pipeline = RendererAPI::create_pipeline(spec);
}

void RootLayer::on_detach() {
    ENG_INFO("RootLayer detached");
}

void RootLayer::on_update(Timestep ts) {
    PROFILE_FUNCTION();

    RenderPassDesc main_pas_desc;
    main_pas_desc.name = "MainScene";
    main_pas_desc.use_depth = false;

    // Color attachment
    RenderPassAttachment color_attachment;
    color_attachment.load_op = wgpu::LoadOp::Clear;
    color_attachment.store_op = wgpu::StoreOp::Store;
    color_attachment.clear_color = {0.1f, 0.1f, 0.1f, 1.0f};
    main_pas_desc.color_attachments.push_back(color_attachment);

    // Depth attachment
    RenderPassAttachment depth_attachment;
    depth_attachment.load_op = wgpu::LoadOp::Clear;
    depth_attachment.store_op = wgpu::StoreOp::Store;
    depth_attachment.clear_depth = 1.0f;
    depth_attachment.read_only_depth = false;
    main_pas_desc.depth_stencil_attachment = depth_attachment;

    auto main_pass = RendererAPI::create_render_pass(main_pas_desc);

    RendererAPI::clear_color(0.1f, 0.1f, 0.1f, 1.0f);
    main_pass->begin();


    main_pass->end();
}

void RootLayer::on_physics_update(Timestep fixed_ts) {}


void RootLayer::on_ui_render() {
    PROFILE_FUNCTION();

    {
        const auto& stats = RendererAPI::get_stats();

        ImGui::Begin("Renderer Stats");
        ImGui::Text("FPS: %.1f", m_displayed_fps);
        ImGui::Text("Frame Time: %.2f ms", m_displayed_frame_time);
        ImGui::Separator();
        ImGui::Text("Draw Calls: %u", stats.draw_calls);
        ImGui::Text("Mesh Count: %u", stats.mesh_count);
        ImGui::Text("Vertex Count: %u", stats.vertex_count);
        ImGui::Text("Index Count: %u", stats.index_count);
        ImGui::End();

    }

    auto show = true;

    ImGui::ShowDemoWindow(&show);

    // ImGui::Begin("Camera Controls");

    // ImGui::End();


}

void RootLayer::on_event(Event& event) {
    PROFILE_FUNCTION();

    EventDispatcher dispatcher(event);


    // Handle keyboard events
    if (event.is_in_category(EventCategoryKeyboard)) {
        ENG_TRACE("RootLayer received keyboard event: {}", event.to_string());

        // Only handle KeyPressedEvent
        if (event.get_event_type() == EventType::KeyPressed) {
            auto& key_event = static_cast<KeyPressedEvent&>(event);
            // if (key_event.get_key_code() == Key::Q) {
            //     ENG_INFO("Q key pressed, closing application...");
            //     Application::get().close();  // Triggers m_running = false
            //     event.handled = true;
            // }
        }
    }

    dispatcher.dispatch<KeyPressedEvent>([&](KeyPressedEvent& e) {
        m_keys[e.get_key_code()] = true;
        return false;
    });

    dispatcher.dispatch<KeyReleasedEvent>([&](KeyReleasedEvent& e) {
        m_keys[e.get_key_code()] = false;
        return false;
    });

    dispatcher.dispatch<MouseButtonPressedEvent>([&](MouseButtonPressedEvent& e) {
        if (e.get_mouse_button() == Mouse::ButtonLeft) {
            m_mouse_dragging = true;
            m_last_mouse_position = Application::get().get_window().get_mouse_position();
        }
        return false;
    });

    dispatcher.dispatch<MouseButtonReleasedEvent>([&](MouseButtonReleasedEvent& e) {
        if (e.get_mouse_button() == Mouse::ButtonLeft) {
            m_mouse_dragging = false;
        }
        return false;
    });
}
