#pragma once
#include "eng/hooks/frontend/gui_hooks.h"
#include "eng/core/assert.h"
#include "eng/core/logger.h"
#include "eng/core/timestep.h"
#include "eng/events/application_event.h"

// ImGui + backends (same as your UILayer does)
#include "eng/hooks/frontend/ui_overlay.h"
#include "eng/platform/window.h"
#include "eng/renderer/renderer_api.h"
#include "eng/ui/implot.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_wgpu.h>
#include <GLFW/glfw3.h>


namespace eng {

void GuiHooks::init(GuiCtx& c, LayerStack<GuiCtx>* stack) {
    static bool s_once = false;
    ENG_CORE_ASSERT(!s_once, "GuiApp already initialized");
    s_once = true;

    // window
    c.get<WindowSys>().win = Window::create(WindowProps("eng Application"));
    auto* glfw = static_cast<GLFWwindow*>(c.get<WindowSys>().win->get_native_window());

    // graphics
    c.get<GfxSys>().ctx = GraphicsContext::create();
    c.get<GfxSys>().ctx->init(c.get<WindowSys>().win.get());
    RendererAPI::init(c.get<GfxSys>().ctx.get());

    // UI overlay lifetime + backends
    c.get<UiOverlay>().on_attach(c);

    // event callback: pre-dispatch close/resize, then bubble to layers
    c.get<WindowSys>().win->set_event_cb([stack, &c](Event& e){
        EventDispatcher d(e);
        d.dispatch<WindowCloseEvent>([&](WindowCloseEvent&){
            c.get<AppState>().running = false; return true;
        });
        d.dispatch<WindowResizeEvent>([&](WindowResizeEvent& ev){
            auto& st = c.get<AppState>();
            st.minimized = (ev.get_width()==0 || ev.get_height()==0);
            auto& gc = *c.get<GfxSys>().ctx;
            gc.configure_surface(gc.get_preferred_format());
            auto [fbw, fbh] = gc.get_framebuffer_size();
            RendererAPI::g_renderer->on_resize(fbw, fbh);
            return false; // let layers handle too
        });

        // Let ImGui eat mouse/keyboard if requested
        c.get<UiOverlay>().filter_event(e);

        // bubble to engine layers (reverse order handled inside)
        stack->run_event(e);
    });
}

inline void GuiHooks::shutdown(GuiCtx& c) {
    c.get<UiOverlay>().on_detach(c);
    RendererAPI::shutdown();
    c.get<GfxSys>().ctx.reset();
    c.get<WindowSys>().win.reset();
}

inline void GuiHooks::begin_frame(GuiCtx& c) {
    RendererAPI::begin_frame();
    c.get<UiOverlay>().begin(c);
}

inline void GuiHooks::end_frame(GuiCtx& c) {
    c.get<UiOverlay>().end(c);
    RendererAPI::end_frame();
    // swap if your GraphicsContext handles present in end_frame, or:
    // c.get<GfxSys>().ctx->swap_buffers();
}

inline void GuiHooks::pump_platform(GuiCtx& c) {
    c.get<WindowSys>().win->on_update();
}

} // namespace eng
