#pragma once

#include "financio/client/trading_worker.h"

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
    TradingWorker m_worker;

	glm::vec2 m_last_mouse_position = {};
	bool m_mouse_dragging = false;

	bool m_keys[512] = {}; // Keyboard state array

	void handle_event(const TradingEvent& ev);

	void setup_pipeline();
	eng::ref<eng::RenderPass> create_main_pass();

	static constexpr int MAX_LOG_LINES = 2000;
    std::deque<std::string> m_event_log;

	std::vector<Bar> m_hist_bars;

	void add_log(const std::string& msg) {
        if (m_event_log.size() >= MAX_LOG_LINES)
            m_event_log.pop_front();
        m_event_log.push_back(msg);
    }

};
