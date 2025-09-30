#include "layer.h"
#include "terra/core/application.h"
#include "terra/debug/profiler.h"
#include "terra/renderer/renderer_api.h"
#include "terra/renderer/orthographic_camera.h"
#include "terra/renderer/perspective_camera.h"

#include "terra/resources/resource_manager.h"
#include "terra/core/timestep.h"
#include "terra/core/timer.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

using namespace terra;

ExampleLayer::ExampleLayer()
    : Layer("ExampleLayer") {}

void ExampleLayer::on_attach() {
    PROFILE_FUNCTION();

    FNC_INFO("ExampleLayer attached");

    float width = Application::get().get_window().get_width();
    float height = Application::get().get_window().get_height();
    float aspect_ratio = width / height;

    m_camera = create_scope<PerspectiveCamera>(
        45.0f,                        // Field of view in degrees
        aspect_ratio,               // width / height
        0.1f,                 // Near plane
        100.0f                   // Far plane
    );

    m_mesh = Mesh::from_file("objects/pyramid.txt");
    m_mesh_2 = Mesh::from_file("objects/webgpu.txt");

    m_shader = RendererAPI::create_shader(
        "shaders/shader.wgsl", 
        "Triangle Shader Module"
    );
    m_shader->vertex_entry = "vs_main";
    m_shader->fragment_entry = "fs_main";

    m_material = RendererAPI::create_material("BasicMaterial", m_shader);
    m_material->define_parameter("ubo", 0, MaterialParamType::Custom, wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment);


    PipelineSpecification spec;
    spec.shader = m_shader;

    VertexBufferLayoutSpec vb = Mesh::get_default_layout();
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

    m_material_instance = m_material->create_instance(pipeline.get());

    generate_pyramid_grid(100, 100, 1.5f); // 10,000 pyramids

}

void ExampleLayer::on_detach() {
    FNC_INFO("ExampleLayer detached");
}

void ExampleLayer::on_update(Timestep ts) {
    PROFILE_FUNCTION();
    FNC_CORE_CRITICAL("Hola 4");


    RenderPassDesc main_pas_desc;
    main_pas_desc.name = "MainScene";

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

        FNC_CORE_INFO("FPS: {:.1f} | Frame Time: {:.2f} ms", m_displayed_fps, m_displayed_frame_time);

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

    // Move camera in local space
    m_camera->move(offset);


    // float time = Timer::elapsed();
    {
        PROFILE_SCOPE("Uniform Creation");

        UniformBlock block;
        block.u_view = m_camera->get_view_matrix();
        block.u_proj = m_camera->get_projection_matrix();
        block.u_time = ts.get_milliseconds();

        m_material_instance->set_parameter("ubo", &block, sizeof(UniformBlock));
    }

    {
        PROFILE_SCOPE("Instances Submit");

        for (const InstanceBlock& instance : m_instances) {
            main_pass->submit(m_mesh, m_material_instance, instance, 0, 1);
        }
    }
    
    main_pass->end();

    if (m_mouse_dragging) {
        Application& app = Application::get();
        glm::vec2 mouse_pos = app.get_window().get_mouse_position();

        glm::vec2 delta = mouse_pos - m_last_mouse_position;
        m_last_mouse_position = mouse_pos;

        float yaw   = delta.x * m_mouse_sensitivity;
        float pitch = -delta.y * m_mouse_sensitivity;

        glm::vec2 rot = m_camera->get_rotation();
        rot.x -= pitch;
        rot.y += yaw;
        m_camera->set_rotation(rot.x, rot.y);
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

    ImGui::Begin("Camera Controls");

    // Position
    {
        glm::vec3 pos = m_camera->get_position();
        if (ImGui::DragFloat3("Position", glm::value_ptr(pos), 0.1f)) {
            m_camera->set_position(pos);
        }
    }

    // Rotation
    {
        glm::vec2 rot = m_camera->get_rotation(); // pitch, yaw
        if (ImGui::DragFloat2("Rotation (Pitch/Yaw)", glm::value_ptr(rot), 1.0f)) {
            m_camera->set_rotation(rot.x, rot.y);
        }
    }

    // Projection params
    {
        float fov = m_camera->get_fov();
        if (ImGui::DragFloat("FOV", &fov, 0.5f, 1.0f, 179.0f)) {
            m_camera->set_fov(fov);
        }

        float aspect = m_camera->get_aspect_ratio();
        // If you want to allow forcing aspect manually:
        if (ImGui::DragFloat("Aspect Ratio", &aspect, 0.01f, 0.1f, 10.0f)) {
            m_camera->set_aspect_ratio(aspect);
        }

        float nearp = m_camera->get_near_plane();
        if (ImGui::DragFloat("Near Plane", &nearp, 0.01f, 0.01f, m_camera->get_far_plane() - 0.01f)) {
            m_camera->set_near_plane(nearp);
        }

        float farp = m_camera->get_far_plane();
        if (ImGui::DragFloat("Far Plane", &farp, 0.1f, m_camera->get_near_plane() + 0.1f, 1000.0f)) {
            m_camera->set_far_plane(farp);
        }
    }

    ImGui::End();


}

void ExampleLayer::on_event(Event& event) {
    PROFILE_FUNCTION();

    using namespace terra;

    EventDispatcher dispatcher(event);


    // Handle keyboard events
    if (event.is_in_category(EventCategoryKeyboard)) {
        FNC_TRACE("ExampleLayer received keyboard event: {}", event.to_string());

        // Only handle KeyPressedEvent
        if (event.get_event_type() == EventType::KeyPressed) {
            auto& key_event = static_cast<KeyPressedEvent&>(event);
            // if (key_event.get_key_code() == Key::Q) {
            //     FNC_INFO("Q key pressed, closing application...");
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
