#include "financio/root_layer.h"

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
    if (m_trader) m_trader->disconnect();
}

void RootLayer::on_attach() {
    PROFILE_FUNCTION();

    ENG_INFO("RootLayer attached — connecting to Trader backend...");

    m_trader = std::make_unique<TraderClient>("localhost:50051");

    m_trader->set_state_handler([this](const StateMessage& msg) {
        // Log messages from backend
        std::string log = "[STATE] id=" + std::to_string(msg.id()) +
                          " type=" + std::to_string(msg.type());
        this->add_log(log);
    });

    if (m_trader->connect()) {
        m_connected = true;
        add_log("Connected to Trader backend (gRPC).");
    } else {
        add_log("Failed to connect to Trader backend.");
    }

#if 0
    setup_pipeline();
#endif

}

void RootLayer::on_detach() {
    ENG_INFO("RootLayer detached");
    if (m_trader) m_trader->disconnect();
    m_connected = false;
}

void RootLayer::on_update(Timestep ts) {
    PROFILE_FUNCTION();

    // TradingEvent ev;
    // while (m_worker.pollEvent(ev)) {
    //     ENG_DEBUG("TradingEvent received: type={}, id={}", (int)ev.type, ev.id);

    //     handle_event(ev);
    // }

#if 0
    ref<RenderPass> main_pass = create_main_pass();
    RendererAPI::clear_color(0.1f, 0.1f, 0.1f, 1.0f);
    main_pass->begin();

    main_pass->end();
#endif
}

void RootLayer::on_physics_update(Timestep fixed_ts) {}


void RootLayer::on_ui_render() {
    ImGui::Begin("Trading Control");

    ImGui::Text("Trader Connection: %s", m_connected ? "Connected" : "Disconnected");

    if (ImGui::Button("Connect Broker")) {
        ENG_DEBUG("CONNECTING ");
        ControlMessage msg;
        msg.set_type(CONTROL_CONNECT);
        auto* c = msg.mutable_connect();
        c->set_host("127.0.0.1");
        c->set_port(4002);
        c->set_client_id(1);
        msg.set_id(1);
        if (m_trader) {
            m_trader->send_control(msg);
        }
        add_log("Sent: ConnectBroker");
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
        m_trader->send_control(msg);
        add_log("Sent: SubscribeMarketData (AAPL)");
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
        m_trader->send_control(msg);
        add_log("Sent: RequestHistoricalData (AAPL)");
    }

    if (ImGui::Button("Disconnect Broker")) {
        ControlMessage msg;
        msg.set_type(CONTROL_DISCONNECT);
        msg.set_id(999);
        m_trader->send_control(msg);
        add_log("Sent: DisconnectBroker");
    }

    ImGui::Separator();

    if (ImGui::Button("Clear Log")) {
        m_event_log.clear();
    }

    ImGui::BeginChild("Trading Log", ImVec2(0, 300), true, ImGuiWindowFlags_HorizontalScrollbar);
    for (const auto& line : m_event_log) {
        ImGui::TextUnformatted(line.c_str());
    }
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);
    ImGui::EndChild();

    ImGui::End();
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


// void RootLayer::handle_event(const TradingEvent& ev) {
//     switch (ev.type) {
//         case TradingEventType::Connected: {
//             std::string msg = "Connected to IB (nextValidId=" + std::to_string(ev.id) + ")";
//             ENG_INFO("{}", msg);
//             add_log(msg);
//             break;
//         }
//         case TradingEventType::Disconnected: {
//             add_log("Disconnected from IB");
//             ENG_WARN("Disconnected from IB");
//             break;
//         }
//         case TradingEventType::Error: {
//             const auto& err = std::get<ErrorData>(ev.payload);
//             std::string msg = "Error " + std::to_string(err.code) + ": " + err.message;
//             ENG_ERROR("{}", msg);
//             add_log(msg);
//             break;
//         }
//         case TradingEventType::TickPrice: {
//             const auto& tick = std::get<TickPriceData>(ev.payload);
//             std::string msg = "TickPrice id=" + std::to_string(ev.id) +
//                               " field=" + std::to_string(tick.field) +
//                               " price=" + std::to_string(tick.price);
//             ENG_TRACE("{}", msg);
//             add_log(msg);
//             break;
//         }
//         case TradingEventType::HistoricalBar: {
//             const auto& bar = std::get<HistoricalBarData>(ev.payload).bar;
//             std::string msg = "HistBar " + std::to_string(ev.id) +
//                               " O=" + std::to_string(bar.open) +
//                               " H=" + std::to_string(bar.high) +
//                               " L=" + std::to_string(bar.low) +
//                               " C=" + std::to_string(bar.close) +
//                               " Vol=" + std::to_string(bar.volume);
//             m_hist_bars.push_back(bar);
//             ENG_DEBUG("{}", msg);
//             add_log(msg);
//             break;
//         }
//         default: {
//             std::string msg = "Unhandled event type=" + std::to_string((int)ev.type);
//             ENG_DEBUG("{}", msg);
//             add_log(msg);
//             break;
//         }
//     }
// }

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
