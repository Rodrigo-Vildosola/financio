#pragma once
#include "eng/hooks/frontend/gui_hooks.h"
#include "eng/core/assert.h"
#include "eng/core/logger.h"
#include "eng/core/timestep.h"
#include "eng/events/application_event.h"

// ImGui + backends (same as your UILayer does)
#include "eng/platform/window.h"
#include "eng/renderer/renderer_api.h"
#include "eng/ui/implot.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_wgpu.h>

namespace eng {

inline void GuiHooks::init(GuiCtx& c, LayerStack<GuiCtx>* stack) {
    static bool s_once = false;
    ENG_CORE_ASSERT(!s_once, "GuiApp already initialized");
    s_once = true;

    // Window
    ENG_CORE_INFO("Creating window");
    c.template get<WindowSys>().win = Window::create(WindowProps("eng Application"));

    // Wire window callback to engine
    c.template get<WindowSys>().win->set_event_cb([stack, &c](Event& e){
        // Pre-dispatch close/resize like old code
        EventDispatcher dispatcher(e);

        dispatcher.dispatch<WindowCloseEvent>([&](WindowCloseEvent&){
            c.template get<AppState>().running = false;
            return true;
        });

        dispatcher.dispatch<WindowResizeEvent>([&](WindowResizeEvent& ev){
            auto& st = c.template get<AppState>();
            st.minimized = (ev.get_width()==0 || ev.get_height()==0);

            // Reconfigure surface and notify renderer
            auto& gc = *c.template get<GfxSys>().ctx;
            gc.configure_surface(gc.get_preferred_format());
            auto [fbw, fbh] = gc.get_framebuffer_size();
            RendererAPI::g_renderer->on_resize(fbw, fbh);
            return false; // let layers see it too
        });

        // Bubble to layers in reverse
        stack->run_event(e);
    });

    // Graphics
    ENG_CORE_INFO("Creating graphics context");
    c.template get<GfxSys>().ctx = GraphicsContext::create();
    c.template get<GfxSys>().ctx->init(c.template get<WindowSys>().win.get());

    RendererAPI::init(c.template get<GfxSys>().ctx.get());

    // Private UI overlay lifetime
    c.template get<UiOverlay>().on_attach(c);
}

inline void GuiHooks::shutdown(GuiCtx& c) {
    c.template get<UiOverlay>().on_detach(c);
    RendererAPI::shutdown();
    c.template get<GfxSys>().ctx.reset();
    c.template get<WindowSys>().win.reset();
}

inline void GuiHooks::begin_frame(GuiCtx& c) {
    RendererAPI::begin_frame();
    c.template get<UiOverlay>().begin(c);
}

inline void GuiHooks::end_frame(GuiCtx& c) {
    c.template get<UiOverlay>().end(c);
    RendererAPI::end_frame();
    // swap if your GraphicsContext handles present in end_frame, or:
    // c.get<GfxSys>().ctx->swap_buffers();
}

inline void GuiHooks::pump_platform(GuiCtx& c) {
    c.template get<WindowSys>().win->on_update();
}

} // namespace eng
