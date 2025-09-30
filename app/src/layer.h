#pragma once

#include <terra/terra.h>

struct alignas(16) UniformBlock {
    glm::mat4 u_view;
    glm::mat4 u_proj;
    float u_time;
};

struct alignas(16) InstanceBlock {
    glm::mat4 model;   // 64 bytes
    glm::vec4 color;   // 16 bytes
};

class ExampleLayer : public terra::Layer
{
public:
	ExampleLayer();
	virtual ~ExampleLayer() = default;

	virtual void on_attach() override;
	virtual void on_detach() override;

	void on_update(terra::Timestep ts) override;
	void on_physics_update(terra::Timestep fixed_ts) override;
	virtual void on_ui_render() override;
	void on_event(terra::Event& e) override;

	void generate_pyramid_grid(int width, int height, float spacing) {
		m_instances.clear();

		for (int x = 0; x < width; ++x) {
			for (int z = 0; z < height; ++z) {
				InstanceBlock instance;
				instance.model = glm::translate(glm::mat4(1.0f), glm::vec3(
					(x - width / 2.0f) * spacing,
					0.0f,
					(z - height / 2.0f) * spacing
				));
				instance.color = glm::vec4(
					static_cast<float>(x) / width,
					0.2f,
					static_cast<float>(z) / height,
					1.0f
				);
				m_instances.push_back(instance);
			}
		}
	}

private:
	terra::ref<terra::Shader> m_shader;
	terra::ref<terra::Material> m_material;
	terra::ref<terra::MaterialInstance> m_material_instance;
	terra::ref<terra::Mesh> m_mesh;
	terra::ref<terra::Mesh> m_mesh_2;

	terra::scope<terra::PerspectiveCamera> m_camera;
	terra::scope<terra::Pipeline> m_pipeline;

	terra::f32 m_cycle = 10;

private:
    float m_fps_accumulator = 0.0f;
    int m_fps_frame_count = 0;
    float m_fps_timer = 0.0f;

    float m_displayed_fps = 0.0f;
    float m_displayed_frame_time = 0.0f;

	glm::vec2 m_last_mouse_position = {};
	bool m_mouse_dragging = false;

	bool m_keys[512] = {}; // Keyboard state array
	float m_camera_speed = 5.0f;  // units per second
	float m_mouse_sensitivity = 0.1f;

	std::vector<InstanceBlock> m_instances;


};
