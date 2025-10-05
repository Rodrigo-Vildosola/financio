#include "financio/root_layer.h"

#include "eng/core/application.h"
#include "eng/core/logger.h"
#include "eng/debug/profiler.h"
#include "eng/renderer/renderer_api.h"

#include "eng/core/timestep.h"
#include "eng/core/timer.h"

#include <imgui.h>
#include <eng/ui/implot.h>

#include <eng/enginio.h>



using namespace eng;

RootLayer::RootLayer() : Layer("RootLayer") {}

void RootLayer::on_attach() {
    PROFILE_FUNCTION();

    m_worker.start();

    // Example: auto-connect when layer attaches
    TradingRequest req;
    req.type = TradingRequestType::Connect;
    req.id   = 0;
    req.payload = ConnectRequest{ "127.0.0.1", 4002, 1 };
    m_worker.postRequest(req);

#if 0
    setup_pipeline();
#endif

}

void RootLayer::on_detach() {
    ENG_INFO("RootLayer detached");

    m_worker.stop();
}

void RootLayer::on_update(Timestep ts) {
    PROFILE_FUNCTION();

    TradingEvent ev;
    while (m_worker.pollEvent(ev)) {
        ENG_DEBUG("TradingEvent received: type={}, id={}", (int)ev.type, ev.id);

        handle_event(ev);
    }

#if 0
    ref<RenderPass> main_pass = create_main_pass();
    RendererAPI::clear_color(0.1f, 0.1f, 0.1f, 1.0f);
    main_pass->begin();

    main_pass->end();
#endif
}

void RootLayer::on_physics_update(Timestep fixed_ts) {}


void RootLayer::on_ui_render() {
    PROFILE_FUNCTION();

    {
        const auto& stats = RendererAPI::get_stats();

        ImGui::Begin("Renderer Stats");
        ImGui::Text("Draw Calls: %u", stats.draw_calls);
        ImGui::Text("Mesh Count: %u", stats.mesh_count);
        ImGui::Text("Vertex Count: %u", stats.vertex_count);
        ImGui::Text("Index Count: %u", stats.index_count);
        ImGui::End();

    }

    auto show = true;

    ImGui::ShowDemoWindow(&show);

    ImGui::Begin("Trading");
    if (ImGui::Button("Subscribe AAPL")) {
        TradingRequest req;
        req.type = TradingRequestType::SubscribeMarketData;
        req.id   = 1; // tickerId
        req.payload = MarketDataRequest{ "AAPL", "SMART", "USD", "STK" };
        m_worker.postRequest(req);
    }

    if (ImGui::Button("Request AAPL Historical")) {
        TradingRequest req;
        req.type = TradingRequestType::RequestHistorical;
        req.id   = 2; // reqId
        req.payload = HistoricalRequest{
            "AAPL", "SMART", "USD", "STK",
            "2 D", "5 mins", "TRADES", 1
        };
        m_worker.postRequest(req);
        m_hist_bars.clear();
    }

    // Plot if we have bars
    if (!m_hist_bars.empty()) {
        if (ImPlot::BeginPlot("AAPL Historical Close")) {
            std::vector<f64> xs, closes;
            xs.reserve(m_hist_bars.size());
            closes.reserve(m_hist_bars.size());

            for (size_t i = 0; i < m_hist_bars.size(); i++) {
                xs.push_back((f64)i); // index as x
                closes.push_back(m_hist_bars[i].close);
            }

            ImPlot::PlotLine("Close", xs.data(), closes.data(), (i32)xs.size());
            ImPlot::EndPlot();
        }
    }
    ImGui::End();

    ImGui::Begin("Trading Log");

    if (ImGui::Button("Clear")) {
        m_event_log.clear();
    }
    ImGui::SameLine();
    static bool autoScroll = true;
    ImGui::Checkbox("Auto-scroll", &autoScroll);

    ImGui::Separator();

    ImGui::BeginChild("LogScrollRegion", ImVec2(0, 0), false,
                      ImGuiWindowFlags_HorizontalScrollbar);

    for (const auto& line : m_event_log) {
        ImGui::TextUnformatted(line.c_str());
    }

    if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();
    ImGui::End();



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


void RootLayer::handle_event(const TradingEvent& ev) {
    switch (ev.type) {
        case TradingEventType::Connected: {
            std::string msg = "Connected to IB (nextValidId=" + std::to_string(ev.id) + ")";
            ENG_INFO("{}", msg);
            add_log(msg);
            break;
        }
        case TradingEventType::Disconnected: {
            add_log("Disconnected from IB");
            ENG_WARN("Disconnected from IB");
            break;
        }
        case TradingEventType::Error: {
            const auto& err = std::get<ErrorData>(ev.payload);
            std::string msg = "Error " + std::to_string(err.code) + ": " + err.message;
            ENG_ERROR("{}", msg);
            add_log(msg);
            break;
        }
        case TradingEventType::TickPrice: {
            const auto& tick = std::get<TickPriceData>(ev.payload);
            std::string msg = "TickPrice id=" + std::to_string(ev.id) +
                              " field=" + std::to_string(tick.field) +
                              " price=" + std::to_string(tick.price);
            ENG_TRACE("{}", msg);
            add_log(msg);
            break;
        }
        case TradingEventType::HistoricalBar: {
            const auto& bar = std::get<HistoricalBarData>(ev.payload).bar;
            std::string msg = "HistBar " + std::to_string(ev.id) +
                              " O=" + std::to_string(bar.open) +
                              " H=" + std::to_string(bar.high) +
                              " L=" + std::to_string(bar.low) +
                              " C=" + std::to_string(bar.close) +
                              " Vol=" + std::to_string(bar.volume);
            ENG_DEBUG("{}", msg);
            add_log(msg);
            break;
        }
        default: {
            std::string msg = "Unhandled event type=" + std::to_string((int)ev.type);
            ENG_DEBUG("{}", msg);
            add_log(msg);
            break;
        }
    }
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
