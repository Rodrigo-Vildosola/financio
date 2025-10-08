#pragma once
#include <string>
#include "eng/core/context.h"
#include "eng/core/caps.h"
#include "eng/core/layer/layer.h"
#include "eng/core/application.h"

// Your existing engine-side headers
#include "eng/platform/window.h"          // eng::Window, Window::create
#include "eng/renderer/renderer_api.h"    // RendererAPI::init/shutdown, g_renderer
#include "eng/renderer/context/context.h" // eng::GraphicsContext

namespace eng {

// App-level state flags that your loop uses
struct AppState {
    bool running   = true;
    bool minimized = false;
};

// Wrap your existing systems in the Context
struct WindowSys  { scope<Window> win; };
struct GfxSys     { scope<GraphicsContext> ctx; };

// Ui overlay for ImGui lifetime. Trivially copyable.
struct UiOverlay {
    void on_attach(auto& c);
    void on_detach(auto& c);
    void on_ui_render(auto&) {} // users draw ImGui in their own layers
    void begin(auto& c);
    void end(auto& c);

    bool m_block_events = true;
    f32 m_time = 0.0f;

    ref<RenderPass> m_ui_pass;
};

// The GUI application Context
using GuiCtx = Context<AppState, WindowSys, GfxSys, UiOverlay>;

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
