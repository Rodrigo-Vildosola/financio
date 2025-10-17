#pragma once

#include "eng/core/base.h"
#include "eng/core/logger.h"
#include "eng/renderer/context/context.h"
#include "eng/platform/window.h"
#include "eng/core/layer.h"
#include "eng/core/layer_stack.h"

#include "eng/runtime/application_base.h"

#include "eng/events/event.h"
#include "eng/events/application_event.h"

#include <glm/glm.hpp>

#include "eng/ui/imgui_layer.h"


int main(int argc, char** argv);

namespace eng {



class Application : public ApplicationBase {
public:
    Application(const std::string& name = "eng Application", CommandLineArgs args = {});
    virtual ~Application();

    void on_event(Event& e);

    CommandLineArgs args() const { return m_command_line_args; }
    
    static Application& get() { return *s_instance; }
    
    UILayer* get_ui_layer() { return m_ui_layer; }
    inline Window& get_window() const { return *m_window; }
    scope<GraphicsContext>& get_context() { return m_context; }
    wgpu::Device get_device() { return m_context->get_native_device(); }

protected:
    void run();

    void begin_frame() override;
    void end_frame() override;
    void pump_platform() override;


private:
    bool on_window_close(WindowCloseEvent& e);
    bool on_window_resize(WindowResizeEvent& e);

    scope<Window>     m_window;
    scope<GraphicsContext>    m_context;
    UILayer*          m_ui_layer = nullptr;

    static Application* s_instance;
};

} // namespace eng
