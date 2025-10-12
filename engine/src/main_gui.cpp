// // main_gui.cpp
// #include "eng/core/application.h"
// #include "eng/core/caps.h"
// #include "eng/core/timestep.h"
// #include "eng/hooks/frontend/gui_complete.h"  // Complete type definitions
// #include "eng/hooks/frontend/gui_hooks.h"     // GuiHooks
// #include <imgui.h>

// using GuiApp = eng::Application<eng::GuiConfig, eng::GuiCtx, eng::GuiHooks>;

// struct Root {
//     void on_attach(eng::GuiCtx& c) { (void)c; }
//     void on_update(eng::GuiCtx&, eng::Timestep dt) { (void)dt; }
//     void on_event(eng::GuiCtx&, eng::Event&) {}
//     void on_ui_render(eng::GuiCtx&) { 
//         ImGui::ShowDemoWindow();
//     }
//     void on_detach(eng::GuiCtx&) {}
// };

// int main(int argc, char** argv) {
//     GuiApp app({argc, argv});
//     app.push_root_layer<Root>();   // attaches now
//     app.run();                     // window events → pre-dispatch → layers
// }
