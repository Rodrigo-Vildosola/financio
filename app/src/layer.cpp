#include "layer.h"
#include "eng/core/application.h"
#include "eng/debug/profiler.h"
#include "eng/renderer/renderer_api.h"

#include "eng/core/timestep.h"
#include "eng/core/timer.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

using namespace eng;

ExampleLayer::ExampleLayer()
    : Layer("ExampleLayer") {}

void ExampleLayer::on_attach() {
    PROFILE_FUNCTION();

    ENG_INFO("ExampleLayer attached");

    float width = Application::get().get_window().get_width();
    float height = Application::get().get_window().get_height();
    float aspect_ratio = width / height;

    m_shader = RendererAPI::create_shader(
        "shaders/shader.wgsl", 
        "Triangle Shader Module"
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

void ExampleLayer::on_detach() {
    ENG_INFO("ExampleLayer detached");
}

void ExampleLayer::on_update(Timestep ts) {
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
    // RendererAPI::begin_scene(*m_camera);

    float delta_time = ts.get_seconds();
    m_fps_accumulator += delta_time;
    m_fps_frame_count++;

    if (m_fps_accumulator >= 1.0f) {
        m_displayed_fps = (float)m_fps_frame_count / m_fps_accumulator;
        m_displayed_frame_time = 1000.0f / m_displayed_fps;  // in milliseconds

        ENG_CORE_INFO("FPS: {:.1f} | Frame Time: {:.2f} ms", m_displayed_fps, m_displayed_frame_time);

        m_fps_accumulator = 0.0f;
        m_fps_frame_count = 0;
    }

    glm::vec3 movement(0.0f);

    if (m_keys[Key::W]) movement.z += 1.0f;
    if (m_keys[Key::S]) movement.z -= 1.0f;
    if (m_keys[Key::A]) movement.x -= 1.0f;
    if (m_keys[Key::D]) movement.x += 1.0f;
    if (m_keys[Key::E]) movement.y += 1.0f;
    if (m_keys[Key::Q]) movement.y -= 1.0f;

    if (glm::length(movement) > 0.0f)
        movement = glm::normalize(movement);

    float dt = ts.get_seconds();
    glm::vec3 offset = movement * m_camera_speed * dt;
    
    main_pass->end();

    if (m_mouse_dragging) {
        Application& app = Application::get();
        glm::vec2 mouse_pos = app.get_window().get_mouse_position();

        glm::vec2 delta = mouse_pos - m_last_mouse_position;
        m_last_mouse_position = mouse_pos;

    }


}

void ExampleLayer::on_physics_update(Timestep fixed_ts) {
    PROFILE_SCOPE("Updating Pyramids");

    float t = Timer::elapsed();
    for (auto& instance : m_instances) {
        glm::vec3 pos = glm::vec3(instance.model[3]);
        float wave = sin(t + pos.x * 0.1f + pos.z * 0.1f);
        instance.model = glm::translate(glm::mat4(1.0f), pos + glm::vec3(0.0f, wave * 0.5f, 0.0f));
    }

}


void ExampleLayer::on_ui_render() {
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

void ExampleLayer::on_event(Event& event) {
    PROFILE_FUNCTION();

    EventDispatcher dispatcher(event);


    // Handle keyboard events
    if (event.is_in_category(EventCategoryKeyboard)) {
        ENG_TRACE("ExampleLayer received keyboard event: {}", event.to_string());

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
