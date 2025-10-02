#pragma once

#include <eng/enginio.h>

struct alignas(16) UniformBlock {
    glm::mat4 u_view;
    glm::mat4 u_proj;
    float u_time;
};

struct alignas(16) InstanceBlock {
    glm::mat4 model;   // 64 bytes
    glm::vec4 color;   // 16 bytes
};

class RootLayer : public eng::Layer
{
public:
	RootLayer();
	virtual ~RootLayer() = default;

	virtual void on_attach() override;
	virtual void on_detach() override;

	void on_update(eng::Timestep ts) override;
	void on_physics_update(eng::Timestep fixed_ts) override;
	virtual void on_ui_render() override;
	void on_event(eng::Event& e) override;

private:
	eng::ref<eng::Shader> m_shader;

	eng::scope<eng::Pipeline> m_pipeline;

	eng::f32 m_cycle = 10;

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
