#pragma once
#include "eng/hooks/frontend/gui_hooks.h"
#include "eng/core/assert.h"
#include "eng/core/logger.h"
#include "eng/core/timestep.h"
#include "eng/events/application_event.h"

// ImGui + backends (same as your UILayer does)
#include <imgui.h>
#include "eng/ui/implot.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_wgpu.h>
#include <GLFW/glfw3.h>

namespace eng {

// ---------- UiOverlay impl ----------
inline void UiOverlay::on_attach(auto& c) {
    // Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad
                    | ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

    // Fonts (use your paths)
    io.Fonts->AddFontFromFileTTF("app/assets/fonts/opensans/OpenSans-Bold.ttf", 18.0f);
    io.FontDefault = io.Fonts->AddFontFromFileTTF("app/assets/fonts/opensans/OpenSans-Regular.ttf", 18.0f);

    ImGui::StyleColorsDark();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        auto& style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Backends
    GLFWwindow* glfw = static_cast<GLFWwindow*>(c.template get<WindowSys>().win->get_native_window());
    ImGui_ImplGlfw_InitForOther(glfw, true);

    auto& gc = *c.template get<GfxSys>().ctx;
    ImGui_ImplWGPU_InitInfo info{};
    info.Device = gc.get_native_device().Get();
    info.RenderTargetFormat = static_cast<WGPUTextureFormat>(gc.get_preferred_format());
    ImGui_ImplWGPU_Init(&info);
}

inline void UiOverlay::on_detach(auto&) {
    ImGui_ImplWGPU_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}

inline void UiOverlay::begin(auto& c) {
    // Begin UI pass
    ImGui_ImplWGPU_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

inline void UiOverlay::end(auto& c) {
    auto& win = *c.template get<WindowSys>().win;
    auto [fbw, fbh] = c.template get<GfxSys>().ctx->get_framebuffer_size();
    const int ww = static_cast<int>(win.get_width());
    const int wh = static_cast<int>(win.get_height());

    ImGuiIO& io = ImGui::GetIO();
    const float sx = ww ? float(fbw)/float(ww) : 1.0f;
    const float sy = wh ? float(fbh)/float(wh) : 1.0f;
    io.DisplaySize = ImVec2(float(fbw), float(fbh));
    io.DisplayFramebufferScale = ImVec2(sx, sy);

    ImGui::Render();

    // Actual drawing is expected inside your renderer’s UI pass encoder.
    // If you keep a pass encoder here, call ImGui_ImplWGPU_RenderDrawData(...)
    // after your encoder is begun. Omitted for brevity, same as your UILayer::end().
}

}
