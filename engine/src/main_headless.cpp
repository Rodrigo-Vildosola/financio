// // main_headless.cpp
// #include "eng/core/application.h"
// #include "eng/core/caps.h"
// #include "eng/core/context.h"
// #include "eng/core/timestep.h"

// // define your headless systems
// struct PhysicsSys {};
// struct JobsSys {};
// using HeadlessCtx = eng::Context<PhysicsSys, JobsSys>;
// using HeadlessApp = eng::Application<eng::HeadlessConfig, HeadlessCtx>;

// struct Root {
//     void on_attach(HeadlessCtx&) {}
//     void on_physics_update(HeadlessCtx&, eng::Timestep dt) { (void)dt; }
//     void on_update(HeadlessCtx&, eng::Timestep dt) { (void)dt; }
//     // on_event/on_ui_render omitted by config
// };

// int main(int argc, char** argv) {
//     HeadlessApp app({argc, argv});
//     app.push_root_layer<Root>();   // on_attach runs immediately
//     app.run();
// }
