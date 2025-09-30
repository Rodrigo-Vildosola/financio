#pragma once

#include "eng/core/base.h"
#include "eng/core/logger.h"
#include "eng/core/context/context.h"
#include "eng/core/window.h"
#include "eng/core/layer.h"
#include "eng/core/layer_stack.h"

#include "eng/events/event.h"
#include "eng/events/application_event.h"

#include <glm/glm.hpp>

#include "eng/ui/imgui_layer.h"


int main(int argc, char** argv);

namespace eng {

class RenderPass;

struct CommandLineArgs {
    i32 count = 0;
    char** args = nullptr;

    const char* operator[](i32 index) const {
        FNC_CORE_ASSERT(index < count, "Index out of bounds for CommandLineArgs");
        return args[index];
    }
};

class Application {
public:
    Application(const std::string& name = "eng Application", CommandLineArgs args = {});
    virtual ~Application();

    void on_event(Event& e);

    void push_layer(Layer* layer);
	void push_overlay(Layer* layer);

    CommandLineArgs get_command_line_args() const { return m_command_line_args; }

    void close();
    
    UILayer* get_ui_layer() { return m_ui_layer; }
    
    static Application& get() { return *s_instance; }
    inline Window& get_window() const { return *m_window; }
    scope<Context>& get_context() { return m_context; }

    wgpu::Device get_device() { return m_context->get_native_device(); }


private:
    void run();

    bool on_window_close(WindowCloseEvent& e);
    bool on_window_resize(WindowResizeEvent& e);

    UILayer* m_ui_layer;
    LayerStack m_layer_stack;
    CommandLineArgs m_command_line_args;

    scope<Window>           m_window;
    scope<Context>    m_context;

    bool m_running = true;
	bool m_minimized = false;

    f32 m_last_frame_time = 0.0f;

    static Application* s_instance;

    friend int ::main(int argc, char** argv);
};

Application* create_application(CommandLineArgs args);  // implemented by the game

} // namespace eng
