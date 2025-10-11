#include "eng/hooks/frontend/ui_overlay.h"
#include "eng/core/assert.h"
#include "eng/core/logger.h"
#include "eng/core/timestep.h"
#include "eng/events/application_event.h"

#include "eng/hooks/frontend/gui_ctx.h"
#include "eng/renderer/render_pass.h"
#include "eng/ui/implot.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_wgpu.h>
#include <GLFW/glfw3.h>

namespace eng {

void UiOverlay::on_attach(GuiCtx& c) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard
                   |  ImGuiConfigFlags_NavEnableGamepad
                   |  ImGuiConfigFlags_DockingEnable
                   |  ImGuiConfigFlags_ViewportsEnable;

    io.Fonts->AddFontFromFileTTF("app/assets/fonts/opensans/OpenSans-Bold.ttf", 18.0f);
    io.FontDefault = io.Fonts->AddFontFromFileTTF("app/assets/fonts/opensans/OpenSans-Regular.ttf", 18.0f);

    ImGui::StyleColorsDark();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        auto& style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    set_dark_theme_colors();

    auto* glfw = static_cast<GLFWwindow*>(c.get<WindowSys>().win->get_native_window());
    ImGui_ImplGlfw_InitForOther(glfw, true);

    auto& gc = *c.get<GfxSys>().ctx;
    ImGui_ImplWGPU_InitInfo info{};
    info.Device = gc.get_native_device().Get();
    info.RenderTargetFormat = static_cast<WGPUTextureFormat>(gc.get_preferred_format());
    ImGui_ImplWGPU_Init(&info);
}


void UiOverlay::on_detach(GuiCtx&) {
    ImGui_ImplWGPU_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}


void UILayer::on_event(Event& e) {
    if (m_block_events)
    {
        ImGuiIO& io = ImGui::GetIO();
        e.handled |= e.is_in_category(EventCategoryMouse) & io.WantCaptureMouse;
        e.handled |= e.is_in_category(EventCategoryKeyboard) & io.WantCaptureKeyboard;
    }
}

void UiOverlay::begin(GuiCtx&) {
    RenderPassDesc ui_pass_desc;
    ui_pass_desc.name = "ImGuiPass";

    RenderPassAttachment color_attachment;
    color_attachment.view = RendererAPI::get_context().get_next_surface_view();
    color_attachment.load_op = wgpu::LoadOp::Load;
    color_attachment.store_op = wgpu::StoreOp::Store;
    ui_pass_desc.color_attachments.push_back(color_attachment);

    m_ui_pass = RendererAPI::create_render_pass(ui_pass_desc);
    m_ui_pass->begin();

    ImGui_ImplWGPU_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UiOverlay::end(GuiCtx& c) {
    auto& win = *c.get<WindowSys>().win;
    auto [fbw, fbh] = c.get<GfxSys>().ctx->get_framebuffer_size();
    const int ww = static_cast<int>(win.get_width());
    const int wh = static_cast<int>(win.get_height());

    ImGuiIO& io = ImGui::GetIO();
    const float sx = ww ? float(fbw)/float(ww) : 1.f;
    const float sy = wh ? float(fbh)/float(wh) : 1.f;
    io.DisplaySize = ImVec2(float(fbw), float(fbh));
    io.DisplayFramebufferScale = ImVec2(sx, sy);

    ImGui::Render();
    const auto& enc = m_ui_pass->get_encoder();
    if (enc) ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), enc.Get());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup);
    }
    m_ui_pass->end();
}

void UiOverlay::filter_event(Event& e) {
    if (!m_block_events) return;
    ImGuiIO& io = ImGui::GetIO();
    e.handled |= e.is_in_category(EventCategoryMouse)    & io.WantCaptureMouse;
    e.handled |= e.is_in_category(EventCategoryKeyboard) & io.WantCaptureKeyboard;
}

void UiOverlay::set_dark_theme_colors() {
    auto& colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

    // Headers
    colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Buttons
    colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
    colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
    colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
}


}
