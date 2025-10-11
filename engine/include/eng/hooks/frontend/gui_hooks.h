#pragma once
#include <string>
#include "eng/core/context.h"
#include "eng/core/caps.h"
#include "eng/core/layer/layer.h"
#include "eng/core/application.h"
#include "eng/hooks/frontend/gui_ctx.h"

// Your existing engine-side headers
// #include "eng/platform/window.h"          // eng::Window, Window::create
// #include "eng/renderer/renderer_api.h"    // RendererAPI::init/shutdown, g_renderer
// #include "eng/renderer/context/context.h" // eng::GraphicsContext

namespace eng {

class Window;
class GraphicsContext;
class RenderPass;


// Hooks that make a GUI app behave like your old Application
struct GuiHooks {
    static void init(GuiCtx& c, LayerStack<GuiCtx>* stack);
    static void shutdown(GuiCtx& c);

    static void begin_frame(GuiCtx& c);
    static void end_frame(GuiCtx& c);

    static void pump_platform(GuiCtx& c);

    // window callback → pre-dispatch → layers
    static bool on_event(void* user, Event& e);
};

// Ready-to-use alias for the templated app
using GuiApp = Application<GuiConfig, GuiCtx, GuiHooks>;

} // namespace eng
