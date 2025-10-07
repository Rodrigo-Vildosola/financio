// #pragma once

// #include "eng/core/base.h"
// #include "eng/core/logger.h"
// #include "eng/core/context/context.h"
// #include "eng/core/window.h"
// #include "eng/core/layer.h"
// #include "eng/core/layer_stack.h"

// #include "eng/runtime/application_base.h"

// #include "eng/events/event.h"
// #include "eng/events/application_event.h"

// #include <glm/glm.hpp>

// #include "eng/ui/imgui_layer.h"


// int main(int argc, char** argv);

// namespace eng {



// class GuiApp : public Application {
// public:
//     GuiApp(const std::string& name = "eng GuiApp", CommandLineArgs args = {});
//     ~GuiApp();

//     void on_event(Event& e);

//     void push_layer(Layer* layer);
// 	void push_overlay(Layer* layer);

//     void close();
    
//     UILayer* get_ui_layer() { return m_ui_layer; }
    
//     static GuiApp& get() { return *s_instance; }
//     inline Window& get_window() const { return *m_window; }
//     scope<Context>& get_context() { return m_context; }
//     wgpu::Device get_device() { return m_context->get_native_device(); }


// private:
//     void run();

//     bool on_window_close(WindowCloseEvent& e);
//     bool on_window_resize(WindowResizeEvent& e);

//     UILayer* m_ui_layer;

//     scope<Window>     m_window;
//     scope<Context>    m_context;

//     static GuiApp* s_instance;

//     friend int ::main(int argc, char** argv);
// };

// GuiApp* create_application(CommandLineArgs args);  // implemented by the application

// } // namespace eng
