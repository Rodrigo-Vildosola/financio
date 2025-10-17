#include "financio/root_layer.h"
#include "financio/core/id.h"

#include "eng/runtime/application.h"

#include "eng/core/logger.h"
#include "eng/core/debug/profiler.h"
#include "eng/renderer/renderer_api.h"

#include "eng/core/timestep.h"
#include "eng/core/timer.h"

#include <imgui.h>
#include <eng/ui/implot.h>

#include <eng/enginio.h>
#include <eng/renderer/renderer_api.h>

using namespace eng;
using namespace financio::trading;

namespace app {

RootLayer::RootLayer() : Layer("RootLayer") {}

RootLayer::~RootLayer() {
    if (m_trader) m_trader->stop();
}

void RootLayer::on_attach() {
    PROFILE_FUNCTION();
    ENG_INFO("RootLayer attached — initializing TraderClient...");

    TraderClient::Options opt;
    opt.address = "localhost:50051";
    opt.heartbeat_period_ms = 2000;
    opt.heartbeat_miss_threshold = 3;

    m_trader = std::make_unique<TraderClient>(opt);

    // State updates
    m_trader->on_state([this](const StateMessage& msg) {
        std::string line = "[STATE] id=" + std::to_string(msg.id()) +
                           " type=" + std::to_string(msg.type());
        add_log(line);
        if (msg.type() == STATE_CONNECTED)
            m_connected.store(true);
        if (msg.type() == STATE_DISCONNECTED)
            m_connected.store(false);
    });

    // Connection lifecycle
    m_trader->on_connected([this] {
        m_connected.store(true);
        add_log("Trader connected.");
    });
    m_trader->on_disconnected([this] {
        m_connected.store(false);
        add_log("Trader disconnected.");
    });
    m_trader->on_error([this](const grpc::Status& st) {
        add_log(std::string("Trader error: ") + st.error_message());
    });

    // Resubscribe hook
    m_trader->set_resubscribe([this] {
        for (const auto& sub : m_active_subs) {
            ControlMessage msg;
            msg.set_type(CONTROL_SUB_MKT);
            auto* req = msg.mutable_sub_mkt_data();
            req->set_symbol(sub.symbol);
            req->set_exchange(sub.exchange);
            req->set_currency(sub.currency);
            req->set_sec_type(sub.sec_type);
            msg.set_id(generate_id());
            m_trader->enqueue(msg);
            add_log("Resubscribed " + sub.symbol);
        }
    });

    m_trader->start();
}

void RootLayer::on_detach() {
    ENG_INFO("RootLayer detached — stopping TraderClient...");
    if (m_trader) m_trader->stop();
    m_connected.store(false);
}

void RootLayer::on_update(Timestep ts) {
    PROFILE_FUNCTION();
    // Nothing here; TraderClient runs in background threads.
}

void RootLayer::on_ui_render() {
    ImGui::Begin("Trading Control");

    bool connected = m_connected.load(std::memory_order_acquire);
    ImGui::Text("Trader Connection: %s", connected ? "Connected" : "Disconnected");

    if (ImGui::Button("Connect Broker")) {
        ControlMessage msg;
        msg.set_type(CONTROL_CONNECT);
        auto* c = msg.mutable_connect();
        c->set_host("127.0.0.1");
        c->set_port(4002);
        c->set_client_id(1);
        msg.set_id(1);
        if (m_trader) m_trader->enqueue(msg);
        add_log("Queued: ConnectBroker");
    }

    if (ImGui::Button("Subscribe AAPL")) {
        ControlMessage msg;
        msg.set_type(CONTROL_SUB_MKT);
        auto* sub = msg.mutable_sub_mkt_data();
        sub->set_symbol("AAPL");
        sub->set_exchange("SMART");
        sub->set_currency("USD");
        sub->set_sec_type("STK");
        msg.set_id(101);
        if (m_trader) m_trader->enqueue(msg);
        add_log("Queued: SubscribeMarketData (AAPL)");
    }

    if (ImGui::Button("Request AAPL Historical")) {
        ControlMessage msg;
        msg.set_type(CONTROL_HISTORICAL);
        auto* req = msg.mutable_req_historical_data();
        req->set_symbol("AAPL");
        req->set_exchange("SMART");
        req->set_currency("USD");
        req->set_sec_type("STK");
        req->set_duration_str("1 D");
        req->set_bar_size("5 mins");
        req->set_what_to_show("TRADES");
        req->set_use_rth(1);
        msg.set_id(102);
        if (m_trader) m_trader->enqueue(msg);
        add_log("Queued: RequestHistoricalData (AAPL)");
    }

    if (ImGui::Button("Disconnect Broker")) {
        ControlMessage msg;
        msg.set_type(CONTROL_DISCONNECT);
        msg.set_id(999);
        if (m_trader) m_trader->enqueue(msg);
        add_log("Queued: DisconnectBroker");
    }

    ImGui::Separator();

    if (ImGui::Button("Clear Log")) m_event_log.clear();

    ImGui::BeginChild("Trading Log", ImVec2(0, 300), true, ImGuiWindowFlags_HorizontalScrollbar);
    for (const auto& line : m_event_log)
        ImGui::TextUnformatted(line.c_str());
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);
    ImGui::EndChild();

    ImGui::End();
}

void RootLayer::on_event(Event& e) {
    PROFILE_FUNCTION();

    EventDispatcher dispatcher(e);

    dispatcher.dispatch<KeyPressedEvent>([&](KeyPressedEvent& ev) {
        m_keys[ev.get_key_code()] = true;
        return false;
    });
    dispatcher.dispatch<KeyReleasedEvent>([&](KeyReleasedEvent& ev) {
        m_keys[ev.get_key_code()] = false;
        return false;
    });
    dispatcher.dispatch<MouseButtonPressedEvent>([&](MouseButtonPressedEvent& ev) {
        if (ev.get_mouse_button() == Mouse::ButtonLeft) {
            m_mouse_dragging = true;
            m_last_mouse_position = Application::get().get_window().get_mouse_position();
        }
        return false;
    });
    dispatcher.dispatch<MouseButtonReleasedEvent>([&](MouseButtonReleasedEvent& ev) {
        if (ev.get_mouse_button() == Mouse::ButtonLeft) m_mouse_dragging = false;
        return false;
    });
}

void RootLayer::add_log(const std::string& msg) {
    if (m_event_log.size() >= MAX_LOG_LINES)
        m_event_log.pop_front();
    m_event_log.push_back(msg);
    ENG_INFO("{}", msg);
}




void RootLayer::setup_pipeline() {
    ref<Shader> shader = RendererAPI::create_shader(
        "shaders/shader.wgsl", 
        "Basic Shader Module"
    );
    shader->vertex_entry = "vs_main";
    shader->fragment_entry = "fs_main";

    PipelineSpecification spec;
    spec.shader = shader;

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

ref<RenderPass> RootLayer::create_main_pass() {

    RenderPassDesc main_pas_desc;
    main_pas_desc.name = "RootScene";
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

    return RendererAPI::create_render_pass(main_pas_desc);
}

}
