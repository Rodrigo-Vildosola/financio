#include "eng/core/application.h"
#include "eng/core/assert.h"
#include "eng/core/logger.h"
#include "eng/core/timestep.h"

#include "eng/core/window.h"
#include "eng/debug/profiler.h"

namespace eng {

Application* Application::s_instance = nullptr;

Application::Application(const std::string& name, CommandLineArgs args)
    : m_command_line_args(args)
{
    PROFILE_FUNCTION();

    FNC_CORE_ASSERT(!s_instance, "Application already exists!");
    s_instance = this;

    FNC_CORE_INFO("Creating window");
    m_window = Window::create(WindowProps(name));
	m_window->set_event_cb(FNC_BIND_EVENT_FN(Application::on_event));

    m_context = Context::create();
    m_context->init(m_window.get());


    #if !defined(FNC_RELEASE)
        FNC_CORE_INFO("Creating ImGui layer");
        m_ui_layer = new UILayer();
        push_overlay(m_ui_layer);
    #endif
}

Application::~Application() {
    PROFILE_FUNCTION();
    RendererAPI::shutdown();
    m_window.reset();
}

void Application::push_layer(Layer* layer) {
    m_layer_stack.push_layer(layer);
    layer->on_attach();
}

void Application::push_overlay(Layer* layer) {
    m_layer_stack.push_overlay(layer);
    layer->on_attach();
}

void Application::on_event(Event& e) {
    EventDispatcher dispatcher(e);
    dispatcher.dispatch<WindowCloseEvent>(FNC_BIND_EVENT_FN(Application::on_window_close));
    dispatcher.dispatch<WindowResizeEvent>(FNC_BIND_EVENT_FN(Application::on_window_resize));

    for (auto it = m_layer_stack.rbegin(); it != m_layer_stack.rend(); ++it)
    {
        if (e.handled) 
            break;
        (*it)->on_event(e);
    }
}


void Application::run() {
    PROFILE_FUNCTION();
    Timer::init();

    const Timestep fixed_dt(1.0f / 60.0f);
    f32 accumulator = 0.0f;

    // render loop
    // -----------
    while (m_running) {
        PROFILE_SCOPE("Render loop");

        f32 time = Timer::elapsed();
        Timestep timestep = time - m_last_frame_time;
        m_last_frame_time = time;

        accumulator += timestep;

        while (accumulator >= fixed_dt) {
            for (Layer* layer : m_layer_stack)
                layer->on_physics_update(fixed_dt);
            accumulator -= fixed_dt;
        }

        if (!m_minimized) {
            for (Layer* layer : m_layer_stack) {
                layer->on_update(timestep);
            }
        }

        #if !defined(FNC_RELEASE)
            m_ui_layer->begin();
            for (Layer* layer : m_layer_stack) {
                layer->on_ui_render();
            }
            m_ui_layer->end();
		#endif

        m_window->on_update();

        // break;
    }

}

void Application::close() {
    m_running = false;
}



bool Application::on_window_resize(WindowResizeEvent& e) {
    PROFILE_FUNCTION();
    if (e.get_width() == 0 || e.get_height() == 0)
    {
        m_minimized = true;
        return false;
    }

    m_minimized = false;

    m_context->configure_surface(m_context->get_preferred_format());

    auto [fb_width, fb_height] = m_context->get_framebuffer_size();

    RendererAPI::g_renderer->on_resize(fb_width, fb_height);

    return false;
}

bool Application::on_window_close(WindowCloseEvent& e) {
    m_running = false;
    return true;
}


}


