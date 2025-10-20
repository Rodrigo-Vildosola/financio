#include "eng/core/base.h"
#include "eng/runtime/application.h"
#include "eng/ui/imgui_layer.h"
#include "eng/core/base.h"
#include "eng/renderer/renderer_api.h"

#include "eng/ui/implot.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_wgpu.h>
#include <GLFW/glfw3.h>



namespace eng {

UILayer::UILayer() : Layer("UILayer") {}

UILayer::~UILayer() {}

void UILayer::on_attach() {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport
    
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;
    io.Fonts->AddFontFromFileTTF("financio/assets/fonts/opensans/OpenSans-Bold.ttf", 18.0f);
    io.FontDefault = io.Fonts->AddFontFromFileTTF("financio/assets/fonts/opensans/OpenSans-Regular.ttf", 18.0f);

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    set_dark_theme_colors();

    Application& app = Application::get();
    GLFWwindow* window = static_cast<GLFWwindow*>(app.get_window().get_native_window());

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOther(window, true);
    ImGui_ImplWGPU_InitInfo info = {};
    info.Device = app.get_device().Get();
    info.RenderTargetFormat = (WGPUTextureFormat) app.get_context()->get_preferred_format();

    ImGui_ImplWGPU_Init(&info);
}

void UILayer::on_detach() {
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

void UILayer::begin() {
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
    // ImGuizmo::BeginFrame();
}

void UILayer::end() {
    ImGuiIO& io = ImGui::GetIO();
    auto& window = Application::get().get_window();
    // auto window = app.get_window();
    auto [fb_width, fb_height] = window.get_framebuffer_size();
    i32 win_w = window.get_width();
    i32 win_h = window.get_height();

    float scale_x = (float)fb_width / (float)win_w;
    float scale_y = (float)fb_height / (float)win_h;


    io.DisplaySize = ImVec2((float)fb_width, (float)fb_height);
    io.DisplayFramebufferScale = ImVec2(scale_x, scale_y);

    // Rendering
    ImGui::Render();
    const auto& pass_encoder = m_ui_pass->get_encoder();

    if (pass_encoder) {
        ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), pass_encoder.Get());
    }

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }

    m_ui_pass->end();
}

// void UILayer::on_ui_render()
// {
// 	static bool show = true;
// 	ImGui::ShowDemoWindow(&show);
// }


void UILayer::set_dark_theme_colors() {
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
