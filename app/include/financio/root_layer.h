#pragma once


#include "financio/rpc/trader_client.h"      // the gRPC client

#include <eng/enginio.h>
#include <queue>

namespace app {


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
	~RootLayer();

	virtual void on_attach() override;
	virtual void on_detach() override;

	void on_update(eng::Timestep ts) override;
	void on_physics_update(eng::Timestep fixed_ts) override;
	virtual void on_ui_render() override;
	void on_event(eng::Event& e) override;

private:
	void setup_pipeline();
	eng::ref<eng::RenderPass> create_main_pass();

    glm::vec2 m_last_mouse_position{};
    bool m_mouse_dragging = false;
    bool m_keys[512] = {};

    static constexpr int MAX_LOG_LINES = 2000;
    std::deque<std::string> m_event_log;
    void add_log(const std::string& msg);

    // Trader client
    std::unique_ptr<TraderClient> m_trader;
    std::atomic<bool> m_connected{false};

    // UI helpers
    void draw_trading_panel();
};

}
